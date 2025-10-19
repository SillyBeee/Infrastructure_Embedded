/**
* @file referee_dev.c
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块
 * @version 0.1
 * @details 提供裁判系统数据的接收与解析功能(该代码基于RoboMaster裁判系统串口协议V1.8.0 2025.4 进行开发)
 * @date 2025-10-01
 * @update 2025-10-10
 * @copyright  Copyright (c) 2025 HDU—PHOENIX
 * @todo
 */


#include "referee_dev.h"



extern uint8_t Referee_Self_ID;//当前机器人的ID
extern uint16_t Referee_SelfClient_ID;//发送者机器人对应的客户端ID