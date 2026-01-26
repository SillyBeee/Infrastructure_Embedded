/**
*   @file dev_dm_imu.h
*   @brief DM_IMU_L1驱动
*   @author HuaCheng Ma
*   @date 2026/1/24
*   @version 0.1
*   @note 目前只支持RS485,CAN通信等待添加
*/
#ifndef DEV_DM_IMU_H
#define DEV_DM_IMU_H

#include "bsp_uart.h"
#define DM_IMU_RX_LEN 19
typedef struct _Dm_IMU_Instance_s
{
    char* topic_name;
    float acc[3];
    float angle_acc[3];
    float angle[3];
    float quat[4];
    UartInstance_s* uart_instance;
} Dm_IMU_Instance_s;

typedef struct {
    char* topic_name;
    UART_HandleTypeDef* uart_handle;
} Dm_IMU_InitConfig_s;


Dm_IMU_Instance_s* Dm_IMU_Register(Dm_IMU_InitConfig_s* config);



#endif //DEV_DM_IMU_H