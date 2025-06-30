#include "user_usart.h"


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        // 发送完成后的处理逻辑
        LOG("USART6 transmission complete\n");
    }
}