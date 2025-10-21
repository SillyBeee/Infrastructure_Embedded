/**
* @file referee_dev.h
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块
 * @version 0.1
 * @details 提供裁判系统数据的接收与解析功能(该代码基于RoboMaster裁判系统串口协议V1.8.0 2025.4 进行开发)
 * @date 2025-10-01
 * @update 2025-10-06
 * @copyright  Copyright (c) 2025 HDU—PHOENIX
 * @todo
 */
#ifndef REFEREE_H
#define REFEREE_H


#include <stdint.h>
#include "referee_protocol.h"
#include "bsp_uart.h"
#include "usart.h"


#define   REF_HEADER_SOF 0xA5 //帧头起始字节
#define   REF_PROTOCOL_HEADER_SIZE 	  5				//帧头长
#define   REF_PROTOCOL_CRC16_SIZE     2       //命令码长度
#define   REF_PROTOCOL_TAIL_SIZE      2	      //帧尾CRC16

#define REF_PROTOCOL_FRAME_MAX_SIZE 128
#define REF_HEADER_CRC_LEN          (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE)
#define REF_HEADER_CRC_CMDID_LEN    (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE + sizeof(uint16_t))
#define REF_HEADER_CMDID_LEN        (REF_PROTOCOL_HEADER_SIZE + sizeof(uint16_t))



typedef struct _packed
{   //帧头格式
    uint8_t SOF;
    uint16_t Data_Length;
    uint8_t Seq;
    uint8_t CRC8;
} frame_header_t;

typedef struct _packed
{   //单帧定义
    frame_header_t frame_header;
    uint16_t cmd_id;
    referee_cmd_id_e data;
    uint16_t  frame_tail;	//CRC16整包校验
}frame_t;


typedef struct _packed
{
    frame_header_t *p_header;
    uint16_t       data_len;
    uint8_t        protocol_packet[REF_PROTOCOL_FRAME_MAX_SIZE];
    uint16_t       index;
}Referee_unpack_data_s;

typedef struct  {
    char* topic_name;
    UART_HandleTypeDef *uart_handle;
}RefereeInitConfig_s;

typedef struct {
    char* topic_name;
    UartInstance_s* uart_instance;
}RefereeInstance_s;

//裁判系统接收解包状态机
typedef  enum {
    STEP_HEADER_SOF  = 0,  //在搜索帧头SOF ing
    STEP_LENGTH_LOW  = 1,  //在解析帧长度低字节ing
    STEP_LENGTH_HIGH = 2,  //在解析帧长度高字节ing
    STEP_FRAME_SEQ   = 3,  //在解析帧序号ing
    STEP_HEADER_CRC8 = 4,  //在解析帧头CRC检验数ing
    STEP_DATA_CRC16  = 5,  //解析数据帧ing
}Referee_Rx_StatusMachine_e;


RefereeInstance_s* Referee_Register(RefereeInitConfig_s* config);
void Referee_Decode_unpack_data(Referee_unpack_data_s* data);



#endif //REFEREE_H
