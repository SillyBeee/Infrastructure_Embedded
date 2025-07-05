#include "WS2812.h"

#include "cmsis_os.h"

uint8_t blue[3] = {0,0,225};


#ifdef USE_TIM_CONTROL
void WS2812::Tim_Init(void){
    //时钟频率为72MHz,计算出的定时器频率为800kHz，并开启DMA传输
    __HAL_TIM_SET_PRESCALER(&this->tim, 0);
    __HAL_TIM_SET_AUTORELOAD(&this->tim,89); // Set the prescaler to 0 for maximum frequency
}

void WS2812::Buffer_Writein(uint16_t *buffer, uint16_t time ,uint32_t color){
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

void WS2812::WS2812_Send(int color){
    if (color == 0){
        Buffer_Writein(this->data_buffer[0], Led_Num, BLUE);
        Buffer_Writein(this->data_buffer[1], next_num, BLUE);
    }else if(color == 1){
        Buffer_Writein(this->data_buffer[0], Led_Num, RED);
        Buffer_Writein(this->data_buffer[1], next_num, RED);
    }
    HAL_TIM_PWM_Start_DMA(&this->tim, this->channel, (uint32_t*)data_buffer[0],Reset_Data +WS2812_Data_Len );
    HAL_Delay(10);
    HAL_TIM_PWM_Start_DMA(&this->tim, this->channel, (uint32_t*)data_buffer[1],Reset_Data + WS2812_Data_Len );
}


//好像有有点用，跑的时候需要改一下
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2 ) {
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
    }
}
#endif



#ifdef USE_SPI_CONTROL
void WS2812::WS2812_Send(uint8_t r, uint8_t g, uint8_t b) const
{
    // LOG("Get RGB Color: R=%d, G=%d, B=%d \n", r, g, b);
    uint8_t txbuf[24];
    uint8_t res = 0;
    for (int i = 0; i < 8; i++)
    {
        txbuf[7-i]  = (((g>>i)&0x01) ? WS2812_HighLevel : WS2812_LowLevel)>>1;
        txbuf[15-i] = (((r>>i)&0x01) ? WS2812_HighLevel : WS2812_LowLevel)>>1;
        txbuf[23-i] = (((b>>i)&0x01) ? WS2812_HighLevel : WS2812_LowLevel)>>1;
    }
    HAL_SPI_Transmit(this->spi, &res, 0, 0xFFFF);
    while (this->spi->State != HAL_SPI_STATE_READY);
    HAL_SPI_Transmit(this->spi, txbuf, 24, 0xFFFF);
    for (int i = 0; i < 100; i++)
    {
        HAL_SPI_Transmit(this->spi, &res, 1, 0xFFFF);
    }
    // LOG("Send RGB Color: R=%d, G=%d, B=%d \n", r, g, b);
};

void WS2812::WS2812_Send_HSV(uint8_t h, uint8_t s, uint8_t v) const
{
    uint8_t r, g, b;
    // LOG("HSV to RGB Color: H=%d, S=%d, V=%d \n", h, s, v);
    if (s == 0)
    {
        r = g = b = v;
    }
    else
    {
        // h (0-255) 分为 6 个区域
        uint8_t region = h / 43;
        // 计算区域内的偏移量, 乘以 6 是为了将范围从 0-42 映射到 0-255
        uint8_t remainder = (h - (region * 43)) * 6;

        // 计算 p, q, t 三个分量
        uint8_t p = (v * (255 - s)) >> 8;
        uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
            case 0: // Red -> Yellow
                r = v; g = t; b = p;
                break;
            case 1: // Yellow -> Green
                r = q; g = v; b = p;
                break;
            case 2: // Green -> Cyan
                r = p; g = v; b = t;
                break;
            case 3: // Cyan -> Blue
                r = p; g = q; b = v;
                break;
            case 4: // Blue -> Magenta
                r = t; g = p; b = v;
                break;
            default: // Magenta -> Red
                r = v; g = p; b = q;
                break;
        }
    }
    // LOG("HSV to RGB Color: R=%d, G=%d, B=%d \n", r, g, b);
    // 调用现有的 RGB 发送函数
    WS2812_Send(r, g, b);
}

void WS2812::Set_RGB_Mode(int speed,int S,int V)
{
    static int H = 0;
    while (1)
    {
        if (H >= 255)
        {
            H = 0;
        }
        this->WS2812_Send_HSV(H, S, V);

        H+=speed;
        osDelay(100);
        // HAL_Delay(50);
    }
}





#endif