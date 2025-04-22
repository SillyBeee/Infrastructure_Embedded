#ifndef MOTOR_H
#define MOTOR_H
#include "stm32f4xx_hal.h"
#include "can.h"

typedef struct
{   

    int16_t MechanicalAngle;  // 机械角度
    int16_t Speed;  // 速度
    int16_t TorqueCurrent;  // 力矩电流
    int8_t Temperature;  // 温度
} MotorStatus;

void CAN_Filter_Init(void);

void Deserialize_Motor_Status(uint8_t *buffer, MotorStatus *status);

void CAN1_Callback(uint8_t* buffer , MotorStatus * status);


#endif //MOTOR_H

