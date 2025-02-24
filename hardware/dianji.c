#include "stm32f10x.h"                  // Device header
#include "pwm.h" 
#include <stdio.h>  

#define AIN1_PIN GPIO_Pin_4              
#define AIN2_PIN GPIO_Pin_5            
//#define PWMA_PIN GPIO_Pin_0           
#define BIN1_PIN GPIO_Pin_6             
#define BIN2_PIN GPIO_Pin_7          
//#define PWMB_PIN GPIO_Pin_1                  

//float pwm1,pwm2;
	
void dianji(void){
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  
    
   GPIO_InitTypeDef GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = AIN1_PIN | AIN2_PIN  | BIN1_PIN | BIN2_PIN ;  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);   
//	GPIO_InitStructure.GPIO_Pin = PWMA_PIN | PWMB_PIN  ;  
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(GPIOB, &GPIO_InitStructure);  	


	pwm();
}
void motor_control(int16_t speed_a, int16_t speed_b,int8_t dir_a, int8_t dir_b) {  
    // A电机
    if (dir_a>0) {  
        GPIO_SetBits(GPIOA, AIN1_PIN);  
        GPIO_ResetBits(GPIOA, AIN2_PIN);  
    } else {  
        GPIO_ResetBits(GPIOA, AIN1_PIN);  
        GPIO_SetBits(GPIOA, AIN2_PIN);  
    }  

    // B电机
    if (dir_b>0) {  
        GPIO_SetBits(GPIOA, BIN1_PIN);  
        GPIO_ResetBits(GPIOA, BIN2_PIN);  
    } else {  
        GPIO_ResetBits(GPIOA, BIN1_PIN);  
        GPIO_SetBits(GPIOA, BIN2_PIN);  
    }  

    // PWM设置
   pwmcontrol(speed_a,speed_b);
}  
