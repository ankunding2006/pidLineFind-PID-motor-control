#ifndef __PID_H
#define __PID_H
void HALL(void);
//int16_t get(void);
//void EXTI0_IRQHandler(void);
//void EXTI1_IRQHandler(void);
//void EXTI2_IRQHandler(void);
//void EXTI3_IRQHandler(void);

float PID_control(float error, int8_t motor_id);


void update_motor_speed(int16_t left, int16_t right);
			

#endif
