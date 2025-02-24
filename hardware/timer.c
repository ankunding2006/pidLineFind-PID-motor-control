//#include "stm32f10x.h"                  // Device header

//void Timer_Init(void){
//	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ?? TIM2 ??  

//    TIM_TimeBaseInitTypeDef TIM_InitStructure;  
//    TIM_InitStructure.TIM_Period = 10000 - 1; // 1000ms  
//    TIM_InitStructure.TIM_Prescaler = 7200 - 1; // 72MHz / 7200 = 10Hz  
//    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
//    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;  
//    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);  

//    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // ??????  
//    TIM_Cmd(TIM2, ENABLE); // ?????  

//    NVIC_InitTypeDef NVIC_InitStructure;  
//    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; // TIM2 ??  
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//    NVIC_Init(&NVIC_InitStructure);  
//}
