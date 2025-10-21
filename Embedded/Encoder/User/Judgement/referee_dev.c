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
#include "alg_crc.h"
#include "bsp_log.h"
#include "robot_config.h"


uint8_t Referee_Self_ID;//当前机器人的ID
uint16_t Referee_SelfClient_ID;//发送者机器人对应的客户端ID
Referee_Rx_StatusMachine_e status = 0;  //当前解包状态机
Referee_unpack_data_s unpack_data_buffer; // 当前解包实时存储的数据

//解析完extern的全局变量，可以在此获取裁判系统数据
game_status_t ext_game_status;
game_result_t ext_game_result;
game_robot_HP_t  ext_game_robot_HP;
event_data_t ext_event_data;
referee_warning_t ext_referee_warning;
dart_info_t ext_dart_info;
robot_status_t ext_robot_status;
power_heat_data_t  ext_power_heat_data;
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
static void Referee_Uart_Callback(UartInstance_s* instance) {
    uint8_t byte = instance->rx_buff[0];
    switch (status) {
        case STEP_HEADER_SOF:
            if (byte == REF_HEADER_SOF) {
                status = STEP_LENGTH_LOW;
                unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            }
            else {
                unpack_data_buffer.index = 0;
            }
            break;

        case STEP_LENGTH_LOW: {
            unpack_data_buffer.data_len = byte;
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_LENGTH_HIGH;
        }break;

        case STEP_LENGTH_HIGH: {
            unpack_data_buffer.data_len |=(byte<<8);
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            //如果解析长度大于最大长度，则废弃
            if (unpack_data_buffer.data_len < (REF_PROTOCOL_FRAME_MAX_SIZE-REF_HEADER_CRC_LEN)) {
                status = STEP_FRAME_SEQ;
            }
            else {
                status = STEP_HEADER_SOF;
                unpack_data_buffer.index = 0;
            }
        }break;

        case  STEP_FRAME_SEQ: {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_HEADER_CRC8;
        }break;

        case STEP_HEADER_CRC8: {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            if (unpack_data_buffer.index == REF_PROTOCOL_FRAME_MAX_SIZE) {
                if (CRC08_Calculate(unpack_data_buffer.protocol_packet, unpack_data_buffer.index - 1)==byte) {
                    status = STEP_FRAME_SEQ;
                }
                else {
                    status = STEP_HEADER_SOF;
                    unpack_data_buffer.index = 0;
                }
            }
        }break;

        case STEP_DATA_CRC16: {
            if (unpack_data_buffer.index <(REF_HEADER_CRC_CMDID_LEN + unpack_data_buffer.data_len)) {
                unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            }
            else {
                //一帧完整结束，状态机与缓存区刷新
                status = STEP_HEADER_SOF;
                unpack_data_buffer.index = 0;
                //解析并存入指定结构体
                //TODO: CRC16校验
                Referee_Decode_unpack_data(&unpack_data_buffer);
            }break;
        }
        default: {
            status = STEP_HEADER_SOF;
            unpack_data_buffer.index = 0;
        }break;
    }
}

void Referee_Decode_unpack_data(Referee_unpack_data_s* data) {
    Log("get referee frame success");
}

RefereeInstance_s* Referee_Register(RefereeInitConfig_s* config) {
    if (config == NULL) {
        return NULL;
    }
    RefereeInstance_s* instance = user_malloc(sizeof(RefereeInstance_s));
    if (instance == NULL) {

        Log_Error("%s : Referee Register Failed, No Memory", config->topic_name);
        return NULL;
    }
    UartInitConfig_s uart_config = {
        .id =   instance,
        .mode = UART_IT_MODE,
        .rx_len = 1,
        .uart_module_callback = Referee_Uart_Callback,
        .uart_handle = config->uart_handle
    };
    UartInstance_s* uart_instance = Uart_Register(&uart_config);
    if (uart_instance == NULL) {
        Log_Error("%s : Uart Register Failed, No Memory", config->topic_name);
    }
    instance->uart_instance = uart_instance;
    instance->topic_name = config->topic_name;
    return instance;
}