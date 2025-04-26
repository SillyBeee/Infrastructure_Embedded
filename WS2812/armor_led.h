#ifndef ARMOR_LED_H
#define ARMOR_LED_H

#include <stdint.h> 

#define High_Data 64
#define Low_Data 36
#define Reset_Data (80)
#define Led_Num (12)
#define next_num (3)
#define Led_Data_Len (24)
#define WS2812_Data_Len (Led_Num * Led_Data_Len)
#define BLUE 0x0000FF
#define RED 0x00FF00
#define GREEN 0xFF0000



/// @brief 定时器初始化
void Tim_Init(void);

/// @brief 将颜色写入缓存区
/// @param buffer 缓存区指针
/// @param time 写入次数(及灯数)
/// @param color 写入颜色
void Buffer_Writein(uint16_t *buffer, uint16_t time, uint32_t color);

/// @brief 初始化缓存区，写完即发送
/// @param color 写入颜色,0代表蓝色，1代表红色
void WS2812_Send(int color);

#endif // ARMOR_LED_H
