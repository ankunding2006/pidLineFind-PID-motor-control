#include "SpeedControl.h"

// 默认速度控制配置
const SpeedConfig SPEED_DEFAULT = {
    .base_speed = 0.7,    // 基础速度0.7m/s
    .min_speed = 0.2,     // 最小速度0.2m/s
    .max_speed = 1.0,     // 最大速度1.0m/s
    .turn_factor = 0.3,   // 转弯减速系数
    .accel_rate = 2.0     // 最大加速度2m/s^2
};

// 速度限幅函数
float constrain(float value, float min, float max) {
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

// 计算电机速度
void calculate_motor_speeds(
    float error,           // 循迹误差 (-1.0 到 1.0)
    float *left_speed,     // 输出左电机速度
    float *right_speed,    // 输出右电机速度
    SpeedConfig *config    // 速度配置参数
) {
    // 1. 基于误差值计算基础速度调整
    float turn_adjust = error * config->turn_factor;
    
    // 2. 计算左右电机目标速度
    *left_speed = config->base_speed - turn_adjust;
    *right_speed = config->base_speed + turn_adjust;
    
    // 3. 速度限幅
    *left_speed = constrain(*left_speed, config->min_speed, config->max_speed);
    *right_speed = constrain(*right_speed, config->min_speed, config->max_speed);
}
