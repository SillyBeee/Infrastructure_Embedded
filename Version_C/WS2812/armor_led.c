#include "armor_led.h"

#include "tim.h"
uint8_t blue[3] = {0,0,225};
uint16_t data_buffer[2][Reset_Data + WS2812_Data_Len] = {0};

void Tim_Init(void){
    //时钟频率为72MHz,计算出的定时器频率为800kHz，并开启DMA传输
    __HAL_TIM_SET_PRESCALER(&htim2, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim2,89); // Set the prescaler to 0 for maximum frequency
}

void Buffer_Writein(uint16_t *buffer, uint16_t time ,uint32_t color){
    for (int i =0;i< time ;i++){
        uint16_t *p = buffer + Reset_Data + (i * Led_Data_Len);
        for (uint8_t j = 0; j < 8; ++j)
            p[j] = (((color << j) & 0X800000) ? High_Data : Low_Data);
        for (uint8_t j = 8; j < 16;++j )
            p[j] = (((color << j) & 0X800000) ? High_Data : Low_Data);
        for (uint8_t j = 16; j < 24; ++j)
            p[j] = (((color << j) & 0X800000) ? High_Data : Low_Data);
    }
}

void WS2812_Send(int color){
    if (color == 0){
        Buffer_Writein(data_buffer[0], Led_Num, BLUE);
        Buffer_Writein(data_buffer[1], next_num, BLUE);
    }else if(color == 1){
        Buffer_Writein(data_buffer[0], Led_Num, RED);
        Buffer_Writein(data_buffer[1], next_num, RED);
    }
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)data_buffer[0],Reset_Data +WS2812_Data_Len );
    HAL_Delay(10);
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)data_buffer[1],Reset_Data + WS2812_Data_Len );
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2) {
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
    }
}



