#ifndef USER_USART_HPP
#define USER_USART_HPP
#include "usart.h"
#include "RTT_Logger.h"
inline bool USART_Msg_Send(UART_HandleTypeDef* uart_handler, uint8_t* data, int length)
{
    if (length > 256) // 假设最大长度为256
    {
        LOG("the message you send is too long\n");
        return false;
    }
    while (HAL_UART_GetState(uart_handler) != HAL_UART_STATE_READY);
    // if (HAL_UART_Transmit(uart_handler, data, length, 1000) == HAL_OK)
    if (HAL_UART_Transmit_DMA(uart_handler, data, length) == HAL_OK)
    {
        LOG("USART message send success\n");
        return true;
    }
    LOG("USART message send failed\n");
    return false;
}



#endif //USER_USART_HPP
