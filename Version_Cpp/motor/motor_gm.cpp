#include "motor_gm.h"

GM6020::GM6020(Motor_CTL_Method mode, CAN_HandleTypeDef* hcan,uint8_t id, Pid_Type pid_type):pid_speed(0.0f, 0.0f, 0.0f, 0.001f, 0.0f),
      pid_angle(0.0f, 0.0f, 0.0f, 0.001f, 0.0f)
{
    this->pid_type  = pid_type;
    this->hcan = hcan;
    this->mode = mode;
    this->id = id;
}



void GM6020::Set_Angle(float angle, float speed)
{
    if (this->pid_type != POSITION_LOOP)
    {
        GM_Error_Handler();
        return;
    }
    Pid_Update(angle);
}

void GM6020::Set_Speed(float speed)
{
    if (this->pid_type != SPEED_LOOP)
    {
        GM_Error_Handler();
        return;
    }
    Pid_Update(speed);


}



void GM6020::Deserialize_Status(const uint8_t* status_buffer)
{
    this->status.Position = (status_buffer[0] << 8) | status_buffer[1];
    status.Speed = (status_buffer[2] << 8) | status_buffer[3];
    status.Current =(status_buffer[4]<< 8) | (status_buffer[5]);
    status.Temperature = status_buffer[6];
}




void GM6020::Pid_Update(float target)
{
    if (this->pid_type == SPEED_LOOP)
    {
        this->pid_speed.Pid_Update(static_cast<float>(this->status.Speed),target);
    }
    else if (this->pid_type == POSITION_LOOP)
    {
        float current_pos  = (this->status.Position*1.0/GM6020_MAX_POSITION)*2*M_PI;
        float target_pos = (target*1.0/GM6020_MAX_POSITION)*2*M_PI;
        this->pid_angle.Pid_Update(current_pos , target_pos);
        this->pid_speed.Pid_Update(this->status.Speed,this->pid_angle.Get_Output());
    }
    else
    {
        GM_Error_Handler();
        return;
    }

}

void GM6020::GM_Error_Handler()
{

}

void GM6020::Set_CTL_Mode(Motor_Mode mode)
{
    this->mode = mode;
}



void GM6020::Set_Pid_Type(Pid_Type pid_type)
{
    this->pid_type = pid_type;
    if (this->pid_type == POSITION_LOOP)
    {
        this->pid_angle.Set_Parameters(0,0,0,0.001);
        this->pid_speed.Set_Parameters(0,0,0,0.001);
    }
    else if (this->pid_type == SPEED_LOOP)
    {
        this->pid_angle.Set_Parameters(0,0,0,0.001);
        this->pid_speed.Set_Parameters(0,0,0,0.001);
    }
}

void GM6020::Set_ID(uint8_t id)
{
    this->id = id;
}
uint8_t GM6020::Get_ID() const
{
    return this->id;
}
