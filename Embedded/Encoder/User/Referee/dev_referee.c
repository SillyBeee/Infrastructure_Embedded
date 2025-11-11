/**
* @file dev_referee.c
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块
 * @version 0.1
 * @details 提供裁判系统数据的接收与解析功能(该代码基于RoboMaster裁判系统串口协议V1.8.0 2025.4 进行开发)
 * @date 2025-10-01
 * @update 2025-10-10
 * @copyright  Copyright (c) 2025 HDU—PHOENIX
 * @todo
 */


#include "dev_referee.h"

#include <string.h>

#include "alg_crc.h"
#include "bsp_log.h"
#include "robot_config.h"

Referee_Rx_StatusMachine_e status = 0; //当前解包状态机
Referee_unpack_data_s unpack_data_buffer; // 当前解包实时存储的数据
frame_header_t unpack_frame_header; //解包时存储当前帧头
uint8_t referee_tx_buffer[137]; //最长可发送包长度 5+2+127+2+1
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
                    Referee_Decode_unpack_data(((RefereeInstance_s*)instance->id),unpack_data_buffer.protocol_packet);
                    ((RefereeInstance_s*)instance->id)->cnt++;
                }

                // Referee_Decode_unpack_data(((RefereeInstance_s*)instance->id),&unpack_data_buffer.protocol_packet);
                // ((RefereeInstance_s*)instance->id)->cnt++;

            }
            break;
        }
    default:
        {
             ((RefereeInstance_s*)instance->id)->Referee_Data_TF = false;
            status = STEP_HEADER_SOF;
            unpack_data_buffer.index = 0;
        }
        break;
    }
}

//数据帧解析函数
void Referee_Decode_unpack_data(RefereeInstance_s* ref_instance, const uint8_t* data)
{
    uint8_t index = 0;
    memcpy(&unpack_frame_header, data, sizeof(frame_header_t));
    index += sizeof(frame_header_t);
    //命令解析
    memcpy(&(unpack_data_buffer.cmd_id), data + index, sizeof(uint16_t));
    index += sizeof(uint16_t);
    switch (unpack_data_buffer.cmd_id)
    {
    case GAME_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_status), data + index, sizeof(game_status_t));
        Log("Get game state");
        break;
    case GAME_RESULT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_result), data + index, sizeof(game_result_t));
        Log("Get game result");
        break;
    case GAME_ROBOT_HP_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_robot_HP), data + index, sizeof(game_robot_HP_t));
        Log("Get game robot HP");
        break;
    case FIELD_EVENTS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_event_data), data + index, sizeof(event_data_t));
        Log("Get event data");
        break;
    case REFEREE_WARNING_CMD_ID: memcpy(&(ref_instance->origin_data.ext_referee_warning), data + index, sizeof(referee_warning_t));
        Log("Get referee warning");
        break;
    case DART_REMAINING_TIME_CMD_ID: memcpy(&(ref_instance->origin_data.ext_dart_info), data + index, sizeof(dart_info_t));
        Log("Get dart info");
        break;
    case ROBOT_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_status), data + index, sizeof(robot_status_t));
        Log("Get robot status");
        break;
    case POWER_HEAT_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_power_heat_data), data + index, sizeof(power_heat_data_t));
        Log("Get power heat data");
        break;
    case ROBOT_POS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_pos), data + index, sizeof(robot_pos_t));
        Log("Get robot pos");
        break;
    case BUFF_MUSK_CMD_ID: memcpy(&(ref_instance->origin_data.ext_buff), data + index, sizeof(buff_t));
        Log("Get buff");
        break;
    case ROBOT_HURT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_hurt_data), data + index, sizeof(hurt_data_t));
        Log("Get hurt");
        break;
    case SHOOT_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_shot_data), data + index, sizeof(shoot_data_t));
        Log("Get shot data");
        break;
    case BULLET_REMAINING_CMD_ID: memcpy(&(ref_instance->origin_data.ext_projectile_allowance), data + index, sizeof(projectile_allowance_t));
        Log("Get projectile allowance");
        break;
    case ROBOT_RFID_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_rfid_status), data + index, sizeof(rfid_status_t));
        Log("Get rfid status");
        break;
    case DART_CLIENT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_dart_cmd), data + index, sizeof(dart_client_cmd_t));
        Log("Get dart client");
        break;
    case GROUND_ROBOT_POSITION_CMD_ID:
        memcpy(&(ref_instance->origin_data.ext_ground_robot_position), data + index, sizeof(ground_robot_position_t));
        Log("Get ground robot position");
        break;
    case LIDAR_PROGRESS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_radar_mark_data), data + index, sizeof(radar_mark_data_t));
        Log("Get radar mark");
        break;
    case SENTRY_INFO_CMD_ID: memcpy(&(ref_instance->origin_data.ext_sentry_info), data + index, sizeof(sentry_info_t));
        Log("Get sentry info");
        break;
    case LIDAR_INFO_CMD_ID: memcpy(&(ref_instance->origin_data.ext_radar_info), data + index, sizeof(radar_info_t));
        Log("Get radar info");
        break;

    case ROBOT_COMMAND_CMD_ID: memcpy(&(ref_instance->origin_data.ext_map_command), data + index, sizeof(map_command_t));
        Log("Get map command");
        break;
    case CUSTOM_CONTROLLER_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_custom_robot_data), data + index, sizeof(custom_robot_data_t));
        Log("Get custom_robot data");
        break;
    case CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_custom_data), data + index,sizeof(custom_robot_data_t));
        Log("client get robot data");
        break;
    case KEYBOARD_MOUSE_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_remote_control), data + index, sizeof(remote_control_t));
        Log("client get remote_control");
        break;
    default:
        Log("Get unknown cmd id %d",unpack_data_buffer.cmd_id);
            break;
    }
    ref_instance->Referee_Data_TF = true;
    // Log("Get Referee frame");
}

RefereeInstance_s* Referee_Register(const RefereeInitConfig_s* config)
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
    instance->Referee_Data_TF = false;

    Referee_Data_Init(instance);
    Log_Passing("%s : Referee instance register success", config->topic_name);
    return instance;
}

void Referee_Data_Init(RefereeInstance_s* ref_instance) {
    memset(&(ref_instance->origin_data),0,sizeof(Referee_Origin_data_s));
}

bool Referee_Send_Msg(RefereeInstance_s* ref_instance, referee_cmd_id_e cmd_id,uint16_t data_length,uint8_t seq, uint8_t *data ) {
    if (ref_instance==NULL) {
        Log_Error("Send_msg: ref_instance is NULL");
        return false;
    }
    referee_tx_buffer[0] = REF_HEADER_SOF;
    referee_tx_buffer[1] = (uint8_t)(data_length & 0xFF);
    referee_tx_buffer[2] = (uint8_t)((data_length >> 8) & 0xFF);
    referee_tx_buffer[3] = seq;
    referee_tx_buffer[4] = CRC08_Calculate(referee_tx_buffer, 4);
    referee_tx_buffer[5] = (uint8_t)(cmd_id & 0xFF);
    referee_tx_buffer[6] = (uint8_t)((cmd_id >> 8) & 0xFF);
    memcpy(referee_tx_buffer+7,data,data_length);
    uint16_t crc16 = CRC16_Calculate(referee_tx_buffer, 7 +  data_length);
    referee_tx_buffer[7+data_length] = (uint8_t)(crc16 & 0xFF);
    referee_tx_buffer[7+data_length+1] = (uint8_t)((crc16 >> 8)&0xFF);
    referee_tx_buffer[7+data_length+2] = '\n';
    return Uart_Transmit(ref_instance->uart_instance, referee_tx_buffer);
}

bool Referee_Get_Data_Status(const RefereeInstance_s* ref_instance) {
    return ref_instance->Referee_Data_TF;
}

bool Referee_Get_Color(RefereeInstance_s* ref_instance) {
    ref_instance->Referee_Self_ID = ref_instance->origin_data.ext_robot_status.robot_id;
    if (ref_instance->origin_data.ext_robot_status.robot_id > 10) {
        return BLUE;
    }
    else {
        return RED;
    }
}

uint8_t Referee_Get_Robot_ID(RefereeInstance_s* ref_instance) {
    ref_instance->Referee_Self_ID = ref_instance->origin_data.ext_robot_status.robot_id;
    return ref_instance->Referee_Self_ID;
}

uint16_t Referee_Get_Client_ID(RefereeInstance_s* ref_instance) {
    bool color = Referee_Get_Color(ref_instance);
    if (color == BLUE) {
        ref_instance->Referee_SelfClient_ID = 0x0164+(ref_instance->Referee_Self_ID - 100);
    }
    else {
        ref_instance->Referee_SelfClient_ID = 0x0100 + ref_instance->Referee_Self_ID;
    }
    return ref_instance->Referee_SelfClient_ID;
}

uint8_t Referee_Get_Game_Status(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_game_status.game_progress;
}

uint16_t Referee_Get_Power_Limit(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.chassis_power_limit;
}

uint16_t Referee_Get_Remain_Energy(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_buff.remaining_energy ;
}

uint16_t Referee_Get_Buffer_Energy(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.buffer_energy;
}

uint8_t Referee_Get_Robot_Level(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.robot_level;
}

uint16_t Referee_Get_Shooter_Heat_42(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.shooter_42mm_barrel_heat;
}

uint16_t Referee_Get_Shooter_Heat_17(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.shooter_17mm_1_barrel_heat;
}

uint16_t Referee_Get_Heat_Limit(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.shooter_barrel_heat_limit;
}

float Referee_Get_Shooter_Speed(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_shot_data.initial_speed;
}

uint16_t Referee_Get_Shooter_Cold(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.shooter_barrel_cooling_value;
}


bool Referee_Send_Custom_Msg_To_Robot( RefereeInstance_s *ref_instance,uint8_t *data , uint16_t length)
{
    //计数
    static int custom_robot_msg_cnt = 0;
    if (length > 32) {
        Log_Error("Custom_Controller to Robot data > 32");
        return false;
    }
    return Referee_Send_Msg(ref_instance, CUSTOM_CONTROLLER_DATA_CMD_ID, length, custom_robot_msg_cnt++, data);
}


bool Referee_Send_Robot_Msg_To_Controller( RefereeInstance_s *ref_instance,uint8_t *data,uint16_t length)
{
    //计数
    static int robot_custom_msg_cnt = 0;
    if (length > 32) {
        Log_Error("Robot to Custom_Controller data > 32");
        return false;
    }
    return Referee_Send_Msg(ref_instance, CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID, length, robot_custom_msg_cnt++, data);

}


