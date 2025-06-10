#include "tim.h"
#include "FreeRTOS.h"
//C板LED PH10：蓝灯  PH11:绿灯  PH12:红灯
#define LED_tim htim5
#define LED_channel_blue TIM_CHANNEL_1
#define LED_channel_green TIM_CHANNEL_2
#define LED_channel_red TIM_CHANNEL_3

uint16_t pwmval = 0;

void LED_Init(void);

void Breath_LED(void);

void RGB_LED(void);