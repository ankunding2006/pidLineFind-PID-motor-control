#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "dianji.h"
#include "pwm.h"
#include <stdio.h>  

#define HALL_A1_PIN GPIO_Pin_0       // A1 
#define HALL_B1_PIN GPIO_Pin_1      // B1
#define HALL_A2_PIN GPIO_Pin_2       // A2
#define HALL_B2_PIN GPIO_Pin_3       // B2

//volatile uint32_t speed_count_a = 0;  // A���������� 
//volatile uint32_t speed_count_b = 0;  // B����������
//float actual_speed_a = 0.0;            // Aʵ���ٶ�  
//float actual_speed_b = 0.0;            // Bʵ���ٶ�  

//float pulses_per_revolution = 12.0; // ÿת�������� 
//float wheel_circumference = 0.1;     // ����������ܳ�  
//float dt = 0.1;                      // ��������������ڣ���λΪ��

// PID���� 
float KP = 0.4;  
float KI = 0.1;  
float KD = 0.0;  

 
//float target_speed = 200.0;  // Ŀ���ٶ�
//float motor_pwm_a = 0;        // ��� A PWMֵ  
//float motor_pwm_b = 0;        // ���B PWMֵ  
float last_error_a = 0;       // A��һ�����  
float last_error_b = 0;       // B��һ����� 
float integral_a = 0;         // A�������ֵ 
float integral_b = 0;         // B�������ֵ

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

// PID����
float PID_control(float error, int8_t motor_id) {  
    if (motor_id == 1) { // A  
        integral_a += error; //���� 
        float derivative = error - last_error_a; // ���� 
        last_error_a = error; // ��¼���
        return KP * error + KI * integral_a + KD * derivative; // PID
    } else { // B 
        integral_b += error; 
        float derivative = error - last_error_b; 
        last_error_b = error; 
        return KP * error + KI * integral_b + KD * derivative; 
    }  
}  

// ���µ���ٶ�
void update_motor_speed(int16_t left, int16_t right) {  
   
    if (left > 100) left = 100;  
    if (right > 100) right = 100;  

    // �����߼�
    int8_t dir_a = left >= 0 ? 1 : 0; // A�������
    int8_t dir_b = right >= 0 ? 1 : 0; // B�������

    // ����ֵ
    left = dir_a ? left : -left;   
    right = dir_b ? right : -right;  

    motor_control(left, right, dir_a, dir_b); 
}  
