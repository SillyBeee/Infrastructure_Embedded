#include "motor_stepper.h"

#include "cancan.h"
#include "user_usart.h"


ZDT_Stepper::ZDT_Stepper(Motor_Mode mode, uint8_t id)
{
    this->hcan = hcan;
    this->id = id;
    this->mode = mode;
}

void ZDT_Stepper::Set_Speed(float speed)
{
    if (this->mode != SPEED_MODE)
    {
        ZDT_Error_Handler();
    }
    while(HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY);
    memset(tx_data, 0, sizeof(tx_data));
    // uint8_t direction = speed > 0 ? 0x01 : 0x00;
    // static uint8_t tx_data[8] = {this->id,0xF6,direction,0x03,0xE8,0x00,0x00,0x6B};
    tx_data[0] = this->id;     // ID
    tx_data[1] = 0xF6;         // 功能码
    tx_data[2] = speed > 0 ? 0x01 : 0x00;
    tx_data[3] = ((int)abs(speed) >> 8) & 0xFF; // 高字节
    tx_data[4] = (int)abs(speed) & 0xFF;        // 低字节
    tx_data[5] = 0x00;      // 加速度为10
    tx_data[6] = 0x00;    // 不启用多机通信
    tx_data[7] = 0x6B;         // 校验码或结束符
    USART_Msg_Send(&huart6, tx_data, 8);
    // CAN_Msg_Send(this->hcan, 8, ((uint32_t)this->id << 8) | (uint32_t)0, tx_data, false);

}


void ZDT_Stepper::Set_Angle(float angle , float speed )
{
    if (this->mode != POSITION_AND_SPEED_MODE)
    {
        ZDT_Error_Handler();
    }
    while(HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY);
    memset(tx_data, 0, sizeof(tx_data));
    tx_data[0] = this->id;     // ID
    tx_data[1] = 0xFD;         // 功能码
    tx_data[2] = speed > 0 ? 0x01 : 0x00;
    tx_data[3] = ((int)abs(speed) >> 8) & 0xFF; // 高字节
    tx_data[4] = (int)abs(speed) & 0xFF;        // 低字节
    tx_data[5] = 0x00;      // 加速度为10
    tx_data[6] = 0x00;   //脉冲数
    tx_data[7] = 0x6B;
    tx_data[8] = 0x00;
    tx_data[9] = 0x00;
    tx_data[10] = 0x01;   //绝对运动
    tx_data[11] = 0x00;   //不启用多机通信
    tx_data[12] = 0x6B;   // 校验码
    USART_Msg_Send(&huart6, tx_data, 13);
}

void ZDT_Stepper::Enable()
{
    while(HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY);
    memset(tx_data, 0, sizeof(tx_data));
    tx_data[0] = this->id;     // ID
    tx_data[1] = 0xF3;         // 功能码
    tx_data[2] = 0xAB;         // 数据内容
    tx_data[3] = 0x01;         // 数据内容
    tx_data[4] = 0x00;         // 数据内容
    tx_data[5] = 0x6B;         // 校验码或结束符

    USART_Msg_Send(&huart6, tx_data, 6);
    // CAN_Msg_Send(this->hcan, 6, ((uint32_t)this->id << 8) | (uint32_t)0, tx_data, false);
}


void ZDT_Stepper::Disable()
{
        while(HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY);
        memset(tx_data, 0, sizeof(tx_data));
        tx_data[0] = this->id;     // ID
        tx_data[1] = 0xF3;         // 功能码
        tx_data[2] = 0xAB;         // 数据内容
        tx_data[3] = 0x00;         // 数据内容
        tx_data[4] = 0x00;         // 数据内容
        tx_data[5] = 0x6B;         // 校验码或结束符
        USART_Msg_Send(&huart6, tx_data, 6);
        // CAN_Msg_Send(this->hcan, 6, ((uint32_t)this->id << 8) | (uint32_t)0, tx_data, false);
}


void ZDT_Stepper::Set_Mode(Motor_Mode mode)
{
    this->mode = mode;
}


void ZDT_Stepper::Set_ID(uint8_t id)
{
    this->id = id;
}

void ZDT_Stepper::Pid_Update(float target) {}

void ZDT_Stepper::ZDT_Error_Handler(){}