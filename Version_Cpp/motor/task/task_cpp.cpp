#include "task_cpp.h"

#include "cancan.h"


// PID ppid(0,0,0,0.001,0);
// void dm_task(void const * argument)
// {
//     CAN_Filter_Init();
//     osDelay(1000);
//     motor.Bind_CAN(&hcan1);
//     motor.Set_Can_ID(0x01);
//     motor.Set_Master_ID(0x00);
//     motor.Set_CTL_Mode(MIT_TORQUE_MODE);
//     motor.Set_Pid_Type(POSITION_LOOP);
//     // motor.Set_Pid_Type(SPEED_LOOP);
//     motor.pid_speed.Set_Parameters(0.8,0.1,0.003,0.001);
//     motor.pid_speed.Set_Max_Output(10);
//     motor.pid_angle.Set_Parameters(61,0,0,0.001);
//     motor.pid_angle.Enable_ZeroCrossingProtection();
//     motor.Enable();
//     HAL_GPIO_WritePin(GPIOH , GPIO_PIN_10,GPIO_PIN_SET);
//     HAL_GPIO_WritePin(GPIOH , GPIO_PIN_11,GPIO_PIN_SET);
//     HAL_GPIO_WritePin(GPIOH , GPIO_PIN_12,GPIO_PIN_SET);
//
//     while (1)
//     {
//         // motor.Set_Speed(setspeed);
//         motor.Set_Angle(setangle,0);
//         osDelay(1);
//     }
// }

GM6020 motor_gm;
DM4310 motor_dm;
float ratio = 1.0;
float setangle = 900;
float setspeed = 3;
uint16_t target_show = 0;
void dm_task(void const* argument)
{
    CAN_Filter_Init();
    osDelay(1000);
    //达妙4310初始化
    // motor_dm.Bind_CAN(&hcan1);
    // motor_dm.Set_Can_ID(0x01);
    // motor_dm.Set_Master_ID(0x11);
    // motor_dm.Set_CTL_Mode(MIT_TORQUE_MODE);
    // motor_dm.Set_Pid_Type(POSITION_LOOP);
    // // motor_dm.Set_Pid_Type(SPEED_LOOP);
    // motor_dm.pid_speed.Set_Parameters(0.95,0.91,0.000,0.001);
    // motor_dm.pid_speed.Set_Max_Output(10);
    // motor_dm.pid_angle.Set_Parameters(39,1.2,0.5,0.001);
    // motor_dm.pid_angle.Enable_ZeroCrossingProtection();
    // motor_dm.Enable();

    //6020初始化
    motor_gm.Bind_CAN(&hcan1);
    motor_gm.Set_ID(4);
    motor_gm.Set_CTL_Mode(VOLTAGE_CTL);
    motor_gm.Set_Pid_Type(POSITION_LOOP);
    // motor_gm.Set_Pid_Type(SPEED_LOOP);
    motor_gm.pid_speed.Set_Parameters(380,6500,0,0.001);
    motor_gm.pid_angle.Set_Parameters(25 , 0, 0,0.001);
    motor_gm.pid_angle.Enable_ZeroCrossingProtection();
    motor_gm.pid_speed.Set_Max_Output(GM6020_MAX_VOLTAGE);

    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_10,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_11,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_12,GPIO_PIN_SET);

    while (1)
    {
        // motor_dm.Set_Speed(setspeed);
        // motor_dm.Set_Angle(setangle,0);
        // motor_gm.Set_Angle(setangle,0);
        // if (motor_gm.status.Position <100 || motor_gm.status.Position>8000)
        // {
        //     ratio = 1.0;
        // }
        // else if (motor_gm.status.Position > 1100)
        // {
        //     ratio = -1.0;
        // }


        // motor_gm.Set_Speed(setspeed);
        // target_show = (uint16_t)(motor_gm / 6.28f *8192.0f);
        motor_gm.Set_Angle(setangle,0);
        GM_Msg_Send(&motor_gm , 1,   &hcan1);
        osDelay(1);
    }

}
