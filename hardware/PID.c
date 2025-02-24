#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "dianji.h"
#include "pwm.h"
#include <stdio.h>  

#define HALL_A1_PIN GPIO_Pin_0       // A1 
#define HALL_B1_PIN GPIO_Pin_1      // B1
#define HALL_A2_PIN GPIO_Pin_2       // A2
#define HALL_B2_PIN GPIO_Pin_3       // B2

//volatile uint32_t speed_count_a = 0;  // A电机脉冲计数 
//volatile uint32_t speed_count_b = 0;  // B电机脉冲计数
//float actual_speed_a = 0.0;            // A实际速度  
//float actual_speed_b = 0.0;            // B实际速度  

//float pulses_per_revolution = 12.0; // 每转的脉冲数 
//float wheel_circumference = 0.1;     // 假设的轮子周长  
//float dt = 0.1;                      // 驱动程序更新周期，单位为秒

// PID参数 
float KP = 0.4;  
float KI = 0.1;  
float KD = 0.0;  

 
//float target_speed = 200.0;  // 目标速度
//float motor_pwm_a = 0;        // 电机 A PWM值  
//float motor_pwm_b = 0;        // 电机B PWM值  
float last_error_a = 0;       // A上一次误差  
float last_error_b = 0;       // B上一次误差 
float integral_a = 0;         // A电机积分值 
float integral_b = 0;         // B电机积分值

void HALL(void) {  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	
    GPIO_InitTypeDef GPIO_InitStructure;  
    GPIO_InitStructure.GPIO_Pin = HALL_A1_PIN | HALL_B1_PIN | HALL_A2_PIN | HALL_B2_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);

    EXTI_InitTypeDef EXTI_InitStructure;  
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
    EXTI_Init(&EXTI_InitStructure);  

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;  
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); 

		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStructure);

}  

// 

// PID控制
float PID_control(float error, int8_t motor_id) {  
    if (motor_id == 1) { // A  
        integral_a += error; //积分 
        float derivative = error - last_error_a; // 导数 
        last_error_a = error; // 记录误差
        return KP * error + KI * integral_a + KD * derivative; // PID
    } else { // B 
        integral_b += error; 
        float derivative = error - last_error_b; 
        last_error_b = error; 
        return KP * error + KI * integral_b + KD * derivative; 
    }  
}  

// 更新电机速度
void update_motor_speed(int16_t left, int16_t right) {  
   
    if (left > 100) left = 100;  
    if (right > 100) right = 100;  

    // 方向逻辑
    int8_t dir_a = left >= 0 ? 1 : 0; // A电机方向
    int8_t dir_b = right >= 0 ? 1 : 0; // B电机方向

    // 正负值
    left = dir_a ? left : -left;   
    right = dir_b ? right : -right;  

    motor_control(left, right, dir_a, dir_b); 
}  
