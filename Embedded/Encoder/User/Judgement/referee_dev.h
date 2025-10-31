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
{
    uint16_t       data_len;
    uint8_t        protocol_packet[REF_PROTOCOL_FRAME_MAX_SIZE];
    uint16_t       index;
    referee_cmd_id_e cmd_id;
}Referee_unpack_data_s;


typedef struct _packed {
    //解析完的原始结构体变量，可以在此获取原始裁判系统数据
    game_status_t ext_game_status;
    game_result_t ext_game_result;
    game_robot_HP_t ext_game_robot_HP;
    event_data_t ext_event_data;
    referee_warning_t ext_referee_warning;
    dart_info_t ext_dart_info;
    robot_status_t ext_robot_status;
    power_heat_data_t ext_power_heat_data;
    robot_pos_t ext_robot_pos;
    buff_t ext_buff;
    hurt_data_t ext_hurt_data;
    shoot_data_t ext_shot_data;
    projectile_allowance_t ext_projectile_allowance;
    rfid_status_t ext_rfid_status;
    dart_client_cmd_t ext_dart_cmd;
    ground_robot_position_t ext_ground_robot_position;
    radar_mark_data_t ext_radar_mark_data;
    sentry_info_t ext_sentry_info;
    radar_info_t ext_radar_info;

    map_command_t ext_map_command;
    custom_robot_data_t ext_custom_robot_data;
    robot_custom_data_t ext_robot_custom_data;
    remote_control_t ext_remote_control;

    //TODO: 0x301机器人交互帧需要特殊处理,下面是子协议的变量
    //0x301机器人交互帧子协议
    sentry_cmd_t ext_sentry_cmd;
    radar_cmd_t ext_radar_cmd;
}Referee_Origin_data_s;


typedef struct  {
    char* topic_name;
    UART_HandleTypeDef *uart_handle;
}RefereeInitConfig_s;

typedef struct {
    char* topic_name;
    UartInstance_s* uart_instance;
    Referee_Origin_data_s origin_data;
    bool Referee_Data_TF ; //当前裁判系统数据是否就绪
    uint8_t Referee_Self_ID; //当前机器人的ID
    uint16_t Referee_SelfClient_ID; //发送者机器人对应的客户端ID
    uint32_t cnt;
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


RefereeInstance_s* Referee_Register(const RefereeInitConfig_s* config);
void Referee_Decode_unpack_data(RefereeInstance_s* ref_instance, const uint8_t* data);
void Referee_Data_Init(RefereeInstance_s* ref_instance);


//辅助函数，从当前已解包裁判系统数据结构体中提取具体数据
bool Referee_Get_Data_Status(const RefereeInstance_s* ref_instance);
bool Referee_Get_Color(RefereeInstance_s* ref_instance);
uint8_t Referee_Get_Robot_ID(RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Client_ID(RefereeInstance_s* ref_instance);
uint8_t Referee_Get_Game_Status(const RefereeInstance_s* ref_instance);

uint8_t Referee_Get_Power_Limit(const RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Remain_Energy(const RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Buffer_Energy(const RefereeInstance_s* ref_instance);
uint8_t Referee_Get_Robot_Level(const RefereeInstance_s* ref_instance);

uint16_t Referee_Get_Shooter_Heat_42(const RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Shooter_Heat_17(const RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Heat_Limit(const RefereeInstance_s* ref_instance);
float Referee_Get_Shooter_Speed(const RefereeInstance_s* ref_instance);
uint16_t Referee_Get_Shooter_Cold(const RefereeInstance_s* ref_instance);


//UI辅助绘画函数




#endif //REFEREE_H
