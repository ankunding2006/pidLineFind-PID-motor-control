#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "dianji.h"
#include "PID.h"
#include "pwm.h"
#include "xunji.h"
#include "bizhang.h"
#include "LinePID.h"
#include "SpeedControl.h"

// 传感器配置
#define SENSOR_LEFT      GPIO_Pin_8
#define SENSOR_MID_LEFT  GPIO_Pin_9
#define SENSOR_MID_RIGHT GPIO_Pin_10
#define SENSOR_RIGHT     GPIO_Pin_11
#define SENSOR_PORT      GPIOA

// 传感器权重定义
const float SENSOR_WEIGHTS[4] = {-3.0f, -1.0f, 1.0f, 3.0f};  // 左->右

// 运动控制参数
#define BASE_SPEED      0.7f    // 基础速度
#define MIN_SPEED       0.2f    // 最小速度
#define TURN_FACTOR     0.3f    // 转向系数
#define CONTROL_PERIOD  10      // 控制周期(ms)
#define DISPLAY_PERIOD  100     // 显示更新周期(ms)

// 全局变量
static uint32_t last_control_time = 0;    // 上次控制时间
static uint32_t last_display_time = 0;    // 上次显示更新时间
static float current_error = 0.0f;        // 当前误差值
static float filtered_error = 0.0f;       // 滤波后误差值

// 编码器计数
volatile uint32_t speed_count_a = 0;
volatile uint32_t speed_count_b = 0;
float actual_speed_a = 0.0f;
float actual_speed_b = 0.0f;

// 滑动平均滤波缓冲区
#define FILTER_SIZE 3
static float error_buffer[FILTER_SIZE] = {0.0f};
static uint8_t buffer_index = 0;

// 添加全局计时变量
static __IO uint32_t TimingDelay = 0;

// SysTick 初始化函数
void SysTick_Init(void) {
    // 配置 SysTick 为 1ms 中断
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);  // 配置失败则死循环
    }
}

// SysTick 中断服务函数
void SysTick_Handler(void) {
    TimingDelay++;
}

// 获取系统运行时间（ms）
uint32_t Get_Tick(void) {
    return TimingDelay;
}

// 读取循迹传感器状态
static void read_line_sensors(uint8_t *sensors) {
    sensors[0] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_LEFT);
    sensors[1] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_MID_LEFT);
    sensors[2] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_MID_RIGHT);
    sensors[3] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_RIGHT);
}

// 计算加权误差值
static float calculate_error(const uint8_t *sensors) {
    float weighted_sum = 0.0f;
    int active_sensors = 0;
    
    for(int i = 0; i < 4; i++) {
        if(sensors[i] == Bit_SET) {
            weighted_sum += SENSOR_WEIGHTS[i];
            active_sensors++;
        }
    }
    
    return (active_sensors > 0) ? (weighted_sum / (active_sensors * 3.0f)) : 0.0f;
}

// 滑动平均滤波
static float error_filter(float new_error) {
    error_buffer[buffer_index] = new_error;
    buffer_index = (buffer_index + 1) % FILTER_SIZE;
    
    float sum = 0.0f;
    for(int i = 0; i < FILTER_SIZE; i++) {
        sum += error_buffer[i];
    }
    return sum / FILTER_SIZE;
}

// 更新速度
static void update_speeds(float error) {
    float left_speed = BASE_SPEED - error * TURN_FACTOR;
    float right_speed = BASE_SPEED + error * TURN_FACTOR;
    
    // 速度限幅
    left_speed = (left_speed < MIN_SPEED) ? MIN_SPEED : left_speed;
    right_speed = (right_speed < MIN_SPEED) ? MIN_SPEED : right_speed;
    
    update_motor_speed(left_speed, right_speed);
}

// 更新显示
static void update_display(void) {
    OLED_ShowNum(2, 1, (int)(actual_speed_b * 100), 5);  // 显示速度
    OLED_ShowNum(4, 1, (int)(filtered_error * 100), 5);  // 显示误差
}

// 主函数
int main(void) {
    // 初始化外设
    OLED_Init();
    dianji();
    pwm();
    HALL();
    xunjiInit();
    bizhang();
    SysTick_Init();  // 添加 SysTick 初始化
    
    // 初始化显示
    OLED_ShowString(1, 1, "Speed:");
    OLED_ShowString(3, 1, "Error:");
    
    // 主循环
    while(1) {
        uint32_t current_time = Get_Tick();  // 替换 HAL_GetTick()
        
        // 控制周期更新
        if(current_time - last_control_time >= CONTROL_PERIOD) {
            uint8_t sensors[4];
            read_line_sensors(sensors);
            
            // 计算误差并滤波
            current_error = calculate_error(sensors);
            filtered_error = error_filter(current_error);
            
            // 更新电机速度
            update_speeds(filtered_error);
            
            last_control_time = current_time;
        }
        
        // 显示更新周期
        if(current_time - last_display_time >= DISPLAY_PERIOD) {
            update_display();
            last_display_time = current_time;
        }
    }
}

// 中断处理函数
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        speed_count_a++;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        speed_count_a++;
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
        speed_count_b++;
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
        speed_count_b++;
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}