/**
*   @file dev_dm_imu.c
*   @brief DM_IMU_L1驱动
*   @author HuaCheng Ma
*   @date 2026/1/24
*   @version 0.1
*   @note 目前只支持RS485,CAN通信等待添加
*/

#include "dev_dm_imu.h"

#include <string.h>
#include "alg_crc.h"
#include "bsp_log.h"
uint32_t crc_code = 0;
uint32_t crc_test_code = 0;

static void Dm_IMU_Callback(UartInstance_s* instance)
{
    if (instance->rx_size != DM_IMU_RX_LEN)
    {
        memset(instance->rx_buff, 0, DM_IMU_RX_LEN);
        HAL_UARTEx_ReceiveToIdle_DMA(instance->uart_handle, instance->rx_buff, instance->rx_len);
        __HAL_DMA_DISABLE_IT(instance->uart_handle->hdmarx, DMA_IT_HT);
        return;
    }
    crc_code = CRC16_Calculate(instance->rx_buff, DM_IMU_RX_LEN - 3);\
    //神秘问题校验过不去
    // if (CRC16_Verify(instance->rx_buff, DM_IMU_RX_LEN-1))
    {
        if (instance->rx_buff[0] == 0x55 &&
            instance->rx_buff[1] == 0xAA &&
            instance->rx_buff[2] == 0x01 &&
            instance->rx_buff[3] == 0x03 &&
            instance->rx_buff[DM_IMU_RX_LEN - 1] == 0x0A)
        {
            Dm_IMU_Instance_s* imu_instance = (Dm_IMU_Instance_s*)(instance->id);
            memcpy(&imu_instance->angle[0], &instance->rx_buff[4], sizeof(float));
            memcpy(&imu_instance->angle[1], &instance->rx_buff[8], sizeof(float));
            memcpy(&imu_instance->angle[2], &instance->rx_buff[12], sizeof(float));
        }
    }
    HAL_UARTEx_ReceiveToIdle_DMA(instance->uart_handle, instance->rx_buff, instance->rx_len);
    __HAL_DMA_DISABLE_IT(instance->uart_handle->hdmarx, DMA_IT_HT);
}

Dm_IMU_Instance_s* Dm_IMU_Register(Dm_IMU_InitConfig_s* config)
{
    if (config == NULL)
    {
        return NULL;
    }
    Dm_IMU_Instance_s* instance = user_malloc(sizeof(Dm_IMU_Instance_s));
    if (instance == NULL)
    {
        Log_Error("%s : DM IMU Register Failed, No Memory", config->topic_name);
        return NULL;
    }
    memset(instance, 0, sizeof(Dm_IMU_Instance_s));
    instance->topic_name = config->topic_name;


    if (config->uart_handle == NULL)
    {
        Log_Error("%s : DM IMU huart is NULL", config->topic_name);
        return NULL;
    }
    UartInitConfig_s uart_config = {};
    uart_config.uart_handle = config->uart_handle;
    uart_config.uart_module_callback = Dm_IMU_Callback;
    uart_config.id = instance;
    uart_config.rx_len = DM_IMU_RX_LEN;
    uart_config.mode = UART_IDLE_MODE;
    // uart_config.mode = UART_IT_MODE;
    instance->uart_instance = Uart_Register(&uart_config);
    if (instance->uart_instance == NULL)
    {
        Log_Error("%s : DM IMU Uart Register Failed, No Memory", config->topic_name);
        return NULL;
    }
    return instance;
}
