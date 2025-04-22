#include "FreeRTOS.h"
#include "buzzer.h"

void buzzer_task(void const * argument){

    PlaytoneArr(2);
    osDelay(250);
    Stoptone();
    PlaytoneArr(3);
    osDelay(250);
    Stoptone;
    Playtone(6);
    osDelay(350);
    Stoptone();
    __HAL_TIM_SET_AUTORELOAD(&Buzzer_tim, 6000);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel, 0);
    HAL_TIM_PWM_Stop(&Buzzer_tim,Buzzer_channel);

	 while(1){
	   osDelay(1000);	 
	 }	 
}