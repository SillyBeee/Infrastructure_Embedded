#include "FreeRTOS.h"
#include "buzzer.h"

void buzzer_task(void const * argument){

    playtone_arr(2);
    osDelay(250);
    stoptone();
    playtone_arr(3);
    osDelay(250);
    stoptone();
    playtone_arr(6);
    osDelay(350);
    stoptone();
    __HAL_TIM_SET_AUTORELOAD(&Buzzer_tim, 6000);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel, 0);
    HAL_TIM_PWM_Stop(&Buzzer_tim,Buzzer_channel);

	 while(1){
	   osDelay(1000);	 
	 }	 
}