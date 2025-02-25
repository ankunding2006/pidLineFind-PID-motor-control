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

// ����������
#define SENSOR_LEFT      GPIO_Pin_8
#define SENSOR_MID_LEFT  GPIO_Pin_9
#define SENSOR_MID_RIGHT GPIO_Pin_10
#define SENSOR_RIGHT     GPIO_Pin_11
#define SENSOR_PORT      GPIOA

// ������Ȩ�ض���
const float SENSOR_WEIGHTS[4] = {-3.0f, -1.0f, 1.0f, 3.0f};  // ��->��

// �˶����Ʋ���
#define BASE_SPEED      0.7f    // �����ٶ�
#define MIN_SPEED       0.2f    // ��С�ٶ�
#define TURN_FACTOR     0.3f    // ת��ϵ��
#define CONTROL_PERIOD  10      // ��������(ms)
#define DISPLAY_PERIOD  100     // ��ʾ��������(ms)

// ȫ�ֱ���
static uint32_t last_control_time = 0;    // �ϴο���ʱ��
static uint32_t last_display_time = 0;    // �ϴ���ʾ����ʱ��
static float current_error = 0.0f;        // ��ǰ���ֵ
static float filtered_error = 0.0f;       // �˲������ֵ

// ����������
volatile uint32_t speed_count_a = 0;
volatile uint32_t speed_count_b = 0;
float actual_speed_a = 0.0f;
float actual_speed_b = 0.0f;

// ����ƽ���˲�������
#define FILTER_SIZE 3
static float error_buffer[FILTER_SIZE] = {0.0f};
static uint8_t buffer_index = 0;

// ���ȫ�ּ�ʱ����
static __IO uint32_t TimingDelay = 0;

// SysTick ��ʼ������
void SysTick_Init(void) {
    // ���� SysTick Ϊ 1ms �ж�
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);  // ����ʧ������ѭ��
    }
}

// SysTick �жϷ�����
void SysTick_Handler(void) {
    TimingDelay++;
}

// ��ȡϵͳ����ʱ�䣨ms��
uint32_t Get_Tick(void) {
    return TimingDelay;
}

// ��ȡѭ��������״̬
static void read_line_sensors(uint8_t *sensors) {
    sensors[0] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_LEFT);
    sensors[1] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_MID_LEFT);
    sensors[2] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_MID_RIGHT);
    sensors[3] = GPIO_ReadInputDataBit(SENSOR_PORT, SENSOR_RIGHT);
}

// �����Ȩ���ֵ
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

// ����ƽ���˲�
static float error_filter(float new_error) {
    error_buffer[buffer_index] = new_error;
    buffer_index = (buffer_index + 1) % FILTER_SIZE;
    
    float sum = 0.0f;
    for(int i = 0; i < FILTER_SIZE; i++) {
        sum += error_buffer[i];
    }
    return sum / FILTER_SIZE;
}

// �����ٶ�
static void update_speeds(float error) {
    float left_speed = BASE_SPEED - error * TURN_FACTOR;
    float right_speed = BASE_SPEED + error * TURN_FACTOR;
    
    // �ٶ��޷�
    left_speed = (left_speed < MIN_SPEED) ? MIN_SPEED : left_speed;
    right_speed = (right_speed < MIN_SPEED) ? MIN_SPEED : right_speed;
    
    update_motor_speed(left_speed, right_speed);
}

// ������ʾ
static void update_display(void) {
    OLED_ShowNum(2, 1, (int)(actual_speed_b * 100), 5);  // ��ʾ�ٶ�
    OLED_ShowNum(4, 1, (int)(filtered_error * 100), 5);  // ��ʾ���
}

// ������
int main(void) {
    // ��ʼ������
    OLED_Init();
    dianji();
    pwm();
    HALL();
    xunjiInit();
    bizhang();
    SysTick_Init();  // ��� SysTick ��ʼ��
    
    // ��ʼ����ʾ
    OLED_ShowString(1, 1, "Speed:");
    OLED_ShowString(3, 1, "Error:");
    
    // ��ѭ��
    while(1) {
        uint32_t current_time = Get_Tick();  // �滻 HAL_GetTick()
        
        // �������ڸ���
        if(current_time - last_control_time >= CONTROL_PERIOD) {
            uint8_t sensors[4];
            read_line_sensors(sensors);
            
            // �������˲�
            current_error = calculate_error(sensors);
            filtered_error = error_filter(current_error);
            
            // ���µ���ٶ�
            update_speeds(filtered_error);
            
            last_control_time = current_time;
        }
        
        // ��ʾ��������
        if(current_time - last_display_time >= DISPLAY_PERIOD) {
            update_display();
            last_display_time = current_time;
        }
    }
}

// �жϴ�����
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