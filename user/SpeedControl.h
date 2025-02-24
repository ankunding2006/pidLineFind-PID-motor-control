#ifndef __SPEED_CONTROL_H
#define __SPEED_CONTROL_H

#include "stm32f10x.h"

// 速度控制参数结构体
typedef struct {
    float base_speed;      // 基础速度 (m/s)
    float min_speed;       // 最小速度
    float max_speed;       // 最大速度
    float turn_factor;     // 转弯系数
    float accel_rate;      // 加速度限制 (m/s^2)
} SpeedConfig;

// 默认配置
extern const SpeedConfig SPEED_DEFAULT;

// 速度计算函数
void calculate_motor_speeds(float error, float *left_speed, float *right_speed, SpeedConfig *config);
float constrain(float value, float min, float max);

#endif