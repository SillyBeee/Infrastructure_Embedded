#include "task_cpp.h"

#include "cancan.h"
DM4310 motor;
float setspeed = 5;
void dm_task(void const * argument)
{
    CAN_Filter_Init();
    osDelay(1000);
    motor.Bind_CAN(&hcan1);
    motor.Set_Can_ID(0x01);
    motor.Set_Master_ID(0x00);
    motor.Set_CTL_Mode(MIT_MODE);
    motor.Set_Pid_Type(SPEED_LOOP);
    motor.pid_speed.Set_Parameters(4,0,10,0.001);
    motor.Enable();
    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_10,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_11,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOH , GPIO_PIN_12,GPIO_PIN_SET);

    while (1)
    {
        motor.Set_Speed(setspeed);
        osDelay(1);
    }
}