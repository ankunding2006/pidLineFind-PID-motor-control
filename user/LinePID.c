#include "LinePID.h"

// 默认PID参数
#define PID_KP_DEFAULT  0.8f    // 保守的初始比例系数
#define PID_KI_DEFAULT  0.1f    // 较小的积分作用
#define PID_KD_DEFAULT  0.2f    // 适度的微分作用
#define INT_LIMIT       0.3f    // 积分限幅
#define OUT_LIMIT       1.0f    // 输出限幅

// 初始化PID控制器
void init_line_pid(LinePID *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->integral_limit = INT_LIMIT;
    pid->output_limit = OUT_LIMIT;
}

// PID控制器更新
float update_line_pid(LinePID *pid, float error) {
    // 1. 计算积分项(带限幅)
    pid->integral += error;
    if(pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    }
    else if(pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    
    // 2. 计算微分项
    float derivative = error - pid->last_error;
    pid->last_error = error;
    
    // 3. PID输出计算
    float output = pid->kp * error +           // 比例项
                  pid->ki * pid->integral +     // 积分项
                  pid->kd * derivative;         // 微分项
    
    // 4. 输出限幅
    if(output > pid->output_limit) {
        output = pid->output_limit;
    }
    else if(output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    
    return output;
}

// PID参数在线调整
void adjust_line_pid_params(LinePID *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

