#include "stm32f10x.h"                  // Device header

//#define LEFT_SENSOR_PIN GPIO_Pin_0     // ×ó PA0  
//#define MIDDLE_LEFT_SENSOR_PIN GPIO_Pin_1 // ÖÐ×ó PA1  
//#define MIDDLE_RIGHT_SENSOR_PIN GPIO_Pin_2 // ÓÒ PA2  
//#define RIGHT_SENSOR_PIN GPIO_Pin_3     // ÖÐÓÒ PA3

void xunjiInit(void) {  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  
    
    GPIO_InitTypeDef GPIO_InitStructure;   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		GPIO_InitStructure.  GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   
}  
