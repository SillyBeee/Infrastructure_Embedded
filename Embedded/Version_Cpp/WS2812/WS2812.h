#ifndef ARMOR_LED_H
#define ARMOR_LED_H
#include <stdint.h>
#include "RTT_Logger.h"
//控制方式选择，支持SPI和PWM控制
// #define USE_TIM_CONTROL
#define USE_SPI_CONTROL

#ifdef USE_TIM_CONTROL
//TIM用
#include "tim.h"
#define High_Data 64
#define Low_Data 36
#define Reset_Data (80)
#define Led_Num (12)
#define next_num (3)
#define Led_Data_Len (24)
#define WS2812_Data_Len (Led_Num * Led_Data_Len)
#endif

#ifdef USE_SPI_CONTROL
//SPI用
#include "spi.h"
#define WS2812_LowLevel    0xC0     // 0码
#define WS2812_HighLevel   0xF0     // 1码

#endif




#define BLUE 0x0000FF
#define RED 0x00FF00
#define GREEN 0xFF0000

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

class WS2812
{
public:
    WS2812() = default;
#ifdef USE_TIM_CONTROL
    /// @brief 定时器初始化
    void Tim_Init(void);

    /// @brief 将颜色写入缓存区
    /// @param buffer 缓存区指针
    /// @param time 写入次数(及灯数)
    /// @param color 写入颜色
    void Buffer_Writein(uint16_t* buffer, uint16_t time, uint32_t color);

    /// @brief 初始化缓存区，写完即发送
    /// @param color 写入颜色,0代表蓝色，1代表红色
    void WS2812_Send(int color);

    uint16_t data_buffer[2][Reset_Data + WS2812_Data_Len] = {0};
    TIM_HandleTypeDef* tim;
    uint32_t channel;
#endif


#ifdef USE_SPI_CONTROL
    void WS2812_Send(uint8_t r, uint8_t g, uint8_t b) const;
    void WS2812_Send_HSV(uint8_t h, uint8_t s, uint8_t v) const;
    //进入RGB炫彩模式
    void Set_RGB_Mode(int speed , int S , int V);
    SPI_HandleTypeDef* spi;
#endif
};









#endif // ARMOR_LED_H
