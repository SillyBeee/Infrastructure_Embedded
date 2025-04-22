#include "led.h"


void LED_Init(void)
{
    HAL_TIM_Base_Init(&LED_tim);
    HAL_TIM_PWM_Start(&LED_tim,LED_channel_blue);
    HAL_TIM_PWM_Start(&LED_tim,LED_channel_green);
    HAL_TIM_PWM_Start(&LED_tim,LED_channel_red);
}
void Breath_LED(void)
{
    while (pwmval < 1000)
    {
        pwmval++;
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_blue, pwmval);
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_green, pwmval);
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_red, pwmval);
        osDelay(2);
    }
    while (pwmval > 0)
    {
        pwmval--;
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_blue, pwmval);
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_green, pwmval);
        __HAL_TIM_SetCompare(&LED_tim,LED_channel_red, pwmval);
        osDelay(2);
    }
    osDelay(200);
}


void RGB_LED(void){
    uint16_t pwmval = 0;
    // 红->绿
    for(pwmval = 0; pwmval < 1000; pwmval++) {
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_red, 1000 - pwmval);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_green, pwmval);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_blue, 0);
        osDelay(2);
    }
    // 绿->蓝
    for(pwmval = 0; pwmval < 1000; pwmval++) {
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_red, 0);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_green, 1000 - pwmval);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_blue, pwmval);
        osDelay(2);
    }
    // 蓝->红
    for(pwmval = 0; pwmval < 1000; pwmval++) {
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_red, pwmval);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_green, 0);
        __HAL_TIM_SetCompare(&LED_tim, LED_channel_blue, 1000 - pwmval);
        osDelay(2);
    }
    osDelay(200);
}