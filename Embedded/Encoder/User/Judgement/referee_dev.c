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

#include <string.h>

#include "alg_crc.h"
#include "bsp_log.h"
#include "robot_config.h"


uint8_t Referee_Self_ID; //当前机器人的ID
uint16_t Referee_SelfClient_ID; //发送者机器人对应的客户端ID

Referee_Rx_StatusMachine_e status = 0; //当前解包状态机
Referee_unpack_data_s unpack_data_buffer; // 当前解包实时存储的数据
frame_header_t unpack_frame_header; //解包时存储当前帧头

//解析完extern的全局变量，可以在此获取裁判系统数据
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


//流式解包函数(因为裁判系统每帧长度不一且可能发生粘包需要流式解包)
static void Referee_Uart_Callback(UartInstance_s* instance)
{
    uint8_t byte = instance->rx_buff[0];
    switch (status)
    {
    case STEP_HEADER_SOF:
        if (byte == REF_HEADER_SOF)
        {
            status = STEP_LENGTH_LOW;
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
        }
        else
        {
            unpack_data_buffer.index = 0;
        }
        break;

    case STEP_LENGTH_LOW:
        {
            unpack_data_buffer.data_len = byte;
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_LENGTH_HIGH;
        }
        break;

    case STEP_LENGTH_HIGH:
        {
            unpack_data_buffer.data_len |= (byte << 8);
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            //如果解析长度大于最大长度，则废弃
            if (unpack_data_buffer.data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_LEN))
            {
                status = STEP_FRAME_SEQ;
            }
            else
            {
                status = STEP_HEADER_SOF;
                unpack_data_buffer.index = 0;
            }
        }
        break;

    case STEP_FRAME_SEQ:
        {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_HEADER_CRC8;
        }
        break;

    case STEP_HEADER_CRC8:
        {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            if (unpack_data_buffer.index == REF_PROTOCOL_HEADER_SIZE)
            {
                if (CRC08_Calculate(unpack_data_buffer.protocol_packet, unpack_data_buffer.index - 1) == byte)
                {
                    status = STEP_DATA_CRC16;
                }
                else
                {
                    status = STEP_HEADER_SOF;
                    unpack_data_buffer.index = 0;
                }
            }
        }
        break;

    case STEP_DATA_CRC16:
        {
            if (unpack_data_buffer.index < (REF_HEADER_CRC_CMDID_LEN + unpack_data_buffer.data_len))
            {
                unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            }
            else
            {
                //一帧完整结束，状态机与缓存区刷新
                status = STEP_HEADER_SOF;
                unpack_data_buffer.index = 0;
                //解析并存入指定结构体
                //TODO: CRC16校验
                if (CRC16_Verify(unpack_data_buffer.protocol_packet, REF_HEADER_CRC_CMDID_LEN+unpack_data_buffer.data_len))
                {
                    Referee_Decode_unpack_data(&unpack_data_buffer.protocol_packet);
                    ((RefereeInstance_s*)instance->id)->cnt++;
                }

                // Referee_Decode_unpack_data(&unpack_data_buffer.protocol_packet);
                // ((RefereeInstance_s*)instance->id)->cnt++;

            }
            break;
        }
    default:
        {
            status = STEP_HEADER_SOF;
            unpack_data_buffer.index = 0;
        }
        break;
    }
}

void Referee_Decode_unpack_data(uint8_t* data)
{
    uint8_t index = 0;
    memcpy(&unpack_frame_header, data, sizeof(frame_header_t));
    index += sizeof(frame_header_t);
    //命令解析
    memcpy(&(unpack_data_buffer.cmd_id), data + index, sizeof(uint16_t));
    index += sizeof(uint16_t);
    switch (unpack_data_buffer.cmd_id)
    {
    case GAME_STATE_CMD_ID: memcpy(&ext_game_status, data + index, sizeof(game_status_t));
        Log("Get game state");
        break;
    case GAME_RESULT_CMD_ID: memcpy(&ext_game_result, data + index, sizeof(game_result_t));
        Log("Get game result");
        break;
    case GAME_ROBOT_HP_CMD_ID: memcpy(&ext_game_robot_HP, data + index, sizeof(game_robot_HP_t));
        Log("Get game robot HP");
        break;
    case FIELD_EVENTS_CMD_ID: memcpy(&ext_event_data, data + index, sizeof(event_data_t));
        Log("Get event data");
        break;
    case REFEREE_WARNING_CMD_ID: memcpy(&ext_referee_warning, data + index, sizeof(referee_warning_t));
        Log("Get referee warning");
        break;
    case DART_REMAINING_TIME_CMD_ID: memcpy(&ext_dart_info, data + index, sizeof(dart_info_t));
        Log("Get dart info");
        break;
    case ROBOT_STATE_CMD_ID: memcpy(&ext_robot_status, data + index, sizeof(robot_status_t));
        Log("Get robot status");
        break;
    case POWER_HEAT_DATA_CMD_ID: memcpy(&ext_power_heat_data, data + index, sizeof(power_heat_data_t));
        Log("Get power heat data");
        break;
    case ROBOT_POS_CMD_ID: memcpy(&ext_robot_pos, data + index, sizeof(robot_pos_t));
        Log("Get robot pos");
        break;
    case BUFF_MUSK_CMD_ID: memcpy(&ext_buff, data + index, sizeof(buff_t));
        Log("Get buff");
        break;
    case ROBOT_HURT_CMD_ID: memcpy(&ext_hurt_data, data + index, sizeof(hurt_data_t));
        Log("Get hurt");
        break;
    case SHOOT_DATA_CMD_ID: memcpy(&ext_shot_data, data + index, sizeof(shoot_data_t));
        Log("Get shot data");
        break;
    case BULLET_REMAINING_CMD_ID: memcpy(&ext_projectile_allowance, data + index, sizeof(projectile_allowance_t));
        Log("Get projectile allowance");
        break;
    case ROBOT_RFID_STATE_CMD_ID: memcpy(&ext_rfid_status, data + index, sizeof(rfid_status_t));
        Log("Get rfid status");
        break;
    case DART_CLIENT_CMD_ID: memcpy(&ext_dart_cmd, data + index, sizeof(dart_client_cmd_t));
        Log("Get dart client");
        break;
    case GROUND_ROBOT_POSITION_CMD_ID:
        memcpy(&ext_ground_robot_position, data + index, sizeof(ground_robot_position_t));
        Log("Get ground robot position");
        break;
    case LIDAR_PROGRESS_CMD_ID: memcpy(&ext_radar_mark_data, data + index, sizeof(radar_mark_data_t));
        Log("Get radar mark");
        break;
    case SENTRY_INFO_CMD_ID: memcpy(&ext_sentry_info, data + index, sizeof(sentry_info_t));
        Log("Get sentry info");
        break;
    case LIDAR_INFO_CMD_ID: memcpy(&ext_radar_info, data + index, sizeof(radar_info_t));
        Log("Get radar info");
        break;

    case ROBOT_COMMAND_CMD_ID: memcpy(&ext_map_command, data + index, sizeof(map_command_t));
        Log("Get map command");
        break;
    case CUSTOM_CONTROLLER_DATA_CMD_ID: memcpy(&ext_custom_robot_data, data + index, sizeof(custom_robot_data_t));
        Log("Get custom_robot data");
        break;
    case CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID: memcpy(&ext_robot_custom_data, data + index,sizeof(custom_robot_data_t));
        Log("client get robot data");
        break;
    case KEYBOARD_MOUSE_DATA_CMD_ID: memcpy(&ext_remote_control, data + index, sizeof(remote_control_t));
        Log("client get remote_control");
        break;
    default:
        Log("Get unknown cmd id %d",unpack_data_buffer.cmd_id);
            break;
    }
    // Log("Get Referee frame");
}

RefereeInstance_s* Referee_Register(RefereeInitConfig_s* config)
{
    if (config == NULL)
    {
        return NULL;
    }
    RefereeInstance_s* instance = user_malloc(sizeof(RefereeInstance_s));
    if (instance == NULL)
    {
        Log_Error("%s : Referee Register Failed, No Memory", config->topic_name);
        return NULL;
    }
    UartInitConfig_s uart_config = {
        .id = instance,
        .mode = UART_IT_MODE,
        .rx_len = 1,
        .uart_module_callback = Referee_Uart_Callback,
        .uart_handle = config->uart_handle
    };
    UartInstance_s* uart_instance = Uart_Register(&uart_config);
    if (uart_instance == NULL)
    {
        Log_Error("%s : Uart Register Failed, No Memory", config->topic_name);
    }
    instance->uart_instance = uart_instance;
    instance->topic_name = config->topic_name;
    instance->cnt = 0;
    Log_Passing("%s : Referee instance register success", config->topic_name);
    return instance;
}
