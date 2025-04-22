#include "motor.h"

#define Motor_CAN hcan1
MotorStatus motor_status = {0};

void CAN_Filter_Init(void)
{
    CAN_FilterTypeDef filterConfig;
    filterConfig.FilterActivation = ENABLE;
    filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

    filterConfig.FilterIdHigh = 0x0000;
    filterConfig.FilterIdLow = 0x0000;

    filterConfig.FilterMaskIdHigh = 0x0000;
    filterConfig.FilterMaskIdLow = 0x0000;

    filterConfig.FilterBank = 0;
    filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&Motor_CAN, &filterConfig);
    HAL_CAN_Start(&Motor_CAN);
    HAL_CAN_ActivateNotification(&Motor_CAN, CAN_IT_RX_FIFO0_MSG_PENDING);  //使能各种中断
    
} 

//解析函数
void Deserialize_Motor_Status(uint8_t *buffer, MotorStatus *status)
{
    status->MechanicalAngle = (buffer[0] << 8) | buffer[1];
    status->Speed = (buffer[2] << 8) | buffer[3];
    status->TorqueCurrent =(buffer[4]<< 8) | (buffer[5]);\
    status->Temperature = buffer[6];
}

//CAN1回调
void CAN1_Callback(uint8_t* buffer , MotorStatus * status)
{
    CAN_RxHeaderTypeDef rx_header;
    HAL_CAN_GetRxMessage(&Motor_CAN, CAN_RX_FIFO0, &rx_header,buffer);
    deserialize_motor_status(buffer, status);
}


//CAN口中断
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan->Instance == CAN1){
        uint8_t RX_Buffer[8] = {0};
        CAN1_Callback(RX_Buffer, &motor_status);
    }


}

