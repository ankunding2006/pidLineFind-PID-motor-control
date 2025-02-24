#ifndef __LINE_PID_H
#define __LINE_PID_H

// PID控制器结构体
typedef struct {
    float kp;              // 比例系数
    float ki;              // 积分系数
    float kd;              // 微分系数
    float integral;        // 积分项
    float last_error;      // 上次误差
    float integral_limit;  // 积分限幅
    float output_limit;    // 输出限幅
} LinePID;

// 函数声明
void init_line_pid(LinePID *pid, float kp, float ki, float kd);
float update_line_pid(LinePID *pid, float error);
void adjust_line_pid_params(LinePID *pid, float kp, float ki, float kd);

#endif