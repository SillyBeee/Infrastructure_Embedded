//
// Created by 29812 on 25-5-14.
//

#ifndef CANCAN_H
#define CANCAN_H
#include <cstring>

#include "can.h"
#include "motor_gm.h"
#include "task_cpp.h"



extern GM6020 motor_gm;
extern DM4310 motor_dm;

inline void CAN_Filter_Init(void)
{
    CAN_FilterTypeDef filterConfig;
    filterConfig.FilterActivation = ENABLE;
    filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

    filterConfig.FilterIdHigh = 0x0000;
    filterConfig.FilterIdLow = 0x0000;

    filterConfig.FilterMaskIdHigh = 0x0000;
    filterConfig.FilterMaskIdLow = 0x0000;
    //RX端初始化
    filterConfig.FilterBank = 0;
    filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &filterConfig);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);  //使能各种中断


}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan->Instance == CAN1){
        CAN_RxHeaderTypeDef rx_header;
        uint8_t motor_status_buffer[8] = {0};
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header,motor_status_buffer);
        if (rx_header.StdId == motor_dm.Get_Master_ID())
        {
            memcpy(motor_dm.status_buffer , motor_status_buffer , 8);
            motor_dm.Deserialize_Status(motor_dm.status_buffer);
        }
        else if (rx_header.StdId == GM6020_STDID_START_ADDRESS + motor_gm.Get_ID())
        {
            memcpy(motor_gm.status_buffer , motor_status_buffer , 8);
            motor_gm.Deserialize_Status(motor_gm.status_buffer);
        }
    }
}
#endif //CANCAN_H