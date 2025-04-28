#include "pid.h"


void Pid_Init(Pid *pid, float Kp, float Ki, float Kd, float dt, Pid_Type type)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->dt = dt;
	pid->target = 0.0f;
    pid->type = type;
    pid->error = 0.0f;
    pid->error_last = 0.0f;
    pid->error_sum = 0.0f;
    pid->output = 0.0f;
    pid->max_output = 20000.0f; // 默认无限幅
    pid->Max_Integral_Limit = 20000.0f;  // 默认无限幅
    pid->Integral_Separation_Threshold = 0.0f;
    pid->Variable_Speed_Integral_Lower_Limit = 0.0f;
    pid->Variable_Speed_Integral_Upper_Limit = 0.0f;
}

void Pid_Set_Kp(Pid *pid, float Kp)
{
    pid->Kp = Kp;
}

void Pid_Set_Ki(Pid *pid, float Ki)
{
    pid->Ki = Ki;
}

void Pid_Set_Kd(Pid *pid, float Kd)
{
    pid->Kd = Kd;
}

void Pid_Set_Max_Output(Pid *pid, float max_output)
{
    pid->max_output = max_output;
}
void Pid_Set_Max_Integral_Limit(Pid *pid, float Max_Integral_Limit) 
{
    pid->Max_Integral_Limit = Max_Integral_Limit;
}

void Pid_Set_Integral_Separation_Threshold(Pid *pid, float Integral_Separation_Threshold) 
{
    pid->Integral_Separation_Threshold = Integral_Separation_Threshold;
}


void Pid_Set_Parameters(Pid *pid, float Kp, float Ki, float Kd, float dt, Pid_Type type)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->dt = dt;
    pid->type = type;
    pid->error = 0.0f;
    pid->error_last = 0.0f;
    pid->error_sum = 0.0f;
    pid->output = 0.0f;
}

void Pid_Update(Pid *pid ,float current_speed , float target_speed){
    if (pid->type == NORMAL_PID) {
        Normal_Pid_Update(pid, current_speed, target_speed);
    }
    else if (pid->type == INTERGRAL_SEPARATION_PID) {
        Intergral_Separation_Pid_Update(pid, current_speed, target_speed);
    }
    else if (pid->type == VARIABLE_SPEED_INTEGRAL_PID) {
        
        //TODO: 还没写捏
    }

}


void Normal_Pid_Update(Pid *pid ,float current_speed , float target_speed)
{   
    pid->error_last = pid->error; 
    pid->error = target_speed - current_speed;
    pid->error_sum += pid->error ;
    pid->output = pid->Kp * pid->error + pid->Ki * pid->error_sum + pid->Kd * (pid->error - pid->error_last) ;
    if (pid->output > pid->max_output){
        pid->output = pid->max_output;
    }
    else if (pid->output < -pid->max_output){
        pid->output = -pid->max_output;
    }
}


void Intergral_Separation_Pid_Update(Pid *pid ,float current_speed , float target_speed)
{ 
    pid->error_last = pid->error; 
    pid->error = target_speed - current_speed;
    pid->error_sum += pid->error * pid->dt;
    //实施积分分离
    if (pid->error > pid->Integral_Separation_Threshold) {
        pid->error_sum = 0.0f; // 积分清零
    } else if (pid->error < -pid->Integral_Separation_Threshold) {
        pid->error_sum = 0.0f; // 积分清零
    } else {
        pid->error_sum += pid->error * pid->dt;
    }

    float I_output = pid->Ki * pid->error_sum;
    //实施积分限幅
    if (I_output > pid->Max_Integral_Limit) {
        I_output = pid->Max_Integral_Limit;
    } else if (I_output < -pid->Max_Integral_Limit) {
        I_output = -pid->Max_Integral_Limit;
    }

    pid->output = pid->Kp * pid->error + I_output + pid->Kd * (pid->error - pid->error_last) / pid->dt;
}

