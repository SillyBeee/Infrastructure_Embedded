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
#define _packed __attribute__((packed))

#include <stdint.h>
#include "referee_protocol.h"
#include "bsp_uart.h"


#define   HEADER_SOF 0xA5 //帧头起始字节
#define   REF_PROTOCOL_HEADER_SIZE 	  5				//帧头长
#define   REF_PROTOCOL_CRC16_SIZE     2       //命令码长度
#define   REF_PROTOCOL_TAIL_SIZE      2	      //帧尾CRC16
#define   REF_DATA_ERROR      0
#define   REF_DATA_CORRECT    1
#define REF_USART_RX_BUF_LENTH 512
#define REF_FIFO_BUF_LENGTH     1024
#define REF_PROTOCOL_FRAME_MAX_SIZE 128
#define REF_HEADER_CRC_LEN          (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE)
#define REF_HEADER_CRC_CMDID_LEN    (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE + sizeof(uint16_t))
#define REF_HEADER_CMDID_LEN        (REF_PROTOCOL_HEADER_SIZE + sizeof(uint16_t))


typedef _packed struct
{   //帧头格式
    uint8_t SOF;
    uint16_t Data_Length;
    uint8_t Seq;
    uint8_t CRC8;
} frame_header_t;

typedef _packed struct
{   //单帧定义
    frame_header_t frame_header;
    uint16_t cmd_id;
    referee_cmd_id_e data;
    uint16_t  frame_tail;	//CRC16整包校验
}frame_t;

typedef enum
{   //帧内关键块的起始字节位置
    FRAME_HEADER         = 0,
    CMD_ID               = 5,
    DATA                 = 7,
    STU_HEADER					 = 7,
    STU_DATA             = 13
} RefereeFrameOffset;

typedef enum
{   //帧头内部字段的起始字节位置
    SOF          = 0,//起始位
    DATA_LENGTH  = 1,//帧内数据长度,根据这个来获取数据长度
    SEQ          = 3,//包序号
    CRC8         = 4 //帧头CRC8校验
} FrameHeaderOffset;




#endif //REFEREE_H
