//
// Created by 29812 on 25-5-14.
//

#ifndef CANCAN_H
#define CANCAN_H
#include <cstring>
#include "RTT_Logger.h"
#include "can.h"
#include "task_cpp.h"


inline bool CAN_Msg_Send(CAN_HandleTypeDef* hcan, int length, uint32_t id, uint8_t* data , bool is_std)
{
    int max_length = 8;
    if (length > max_length)
    {
        LOG("the message you send is too long\n");
        return false;
    }
    CAN_TxHeaderTypeDef tx_header;
    if (is_std)
    {
        tx_header.StdId = id; // 设置标准帧ID
        tx_header.IDE = CAN_ID_STD; // 标准帧
    }
    else
    {
        tx_header.StdId = 0x00;
        tx_header.ExtId = id; // 设置扩展帧ID
        tx_header.IDE = CAN_ID_EXT; // 扩展帧

    }
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = length;
    uint8_t tx_data[length];
    uint32_t tx_mailbox;
    for  (int i = 0; i < length; i++)
    {
        tx_data[i] = data[i];
    }
    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK)
    {
        if (is_std) {
            LOG("STD message send success\n");
        } else {
            LOG("EXT message send success\n");
        }
        return true;
    }
    return false;
}


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
    LOG("CAN init success\n");

}


inline void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan->Instance == CAN1){
        CAN_RxHeaderTypeDef rx_header;
        uint8_t motor_status_buffer[8] = {0};
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header,motor_status_buffer);
        LOG("get message\n");
    }
}
#endif //CANCAN_H