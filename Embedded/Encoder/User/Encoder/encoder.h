/**
* @file encoder.h
 * @author Ma HuaCheng (3576527475a@gmail.com)
 * @brief  欧艾迪编码器(CAN)驱动代码 型号:OID-C3806D-10S1S
 * @version 0.1
 * @details 编码器角度读取等功能
 * @date 2025-09-27
 * @copyright  Copyright (c) 2025 HDU—PHOENIX
 */
#ifndef ENCODER_H
#define ENCODER_H
#include "bsp_can.h"
#include "bsp_log.h"


typedef struct _EncoderInitConfig_s {
    char* topic_name;      //实例名称
    uint16_t encoder_address;  //编码器地址
    CanInitConfig_s *can_config; //绑定的Can配置参数
    bool is_auto_refresh; //是否开启角度值自动刷新
    int refresh_time; //自动刷新周期，单位ms
} EncoderInitConfig_s;


typedef struct _EncoderInstance_s {
    //配置文件相关参数
    char* topic_name;
    uint16_t encoder_address;
    CanInstance_s *can_instance;
    bool is_auto_refresh;
    int refresh_time;

    int angle;  //解码角度值
} EncoderInstance_s;


EncoderInstance_s *Encoder_Register(const EncoderInitConfig_s* config);

bool Encoder_Angle_Refresh(EncoderInstance_s* instance);



#endif //ENCODER_H
