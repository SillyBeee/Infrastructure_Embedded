#include "buzzer.h"


void Buzzer_Init(void)
{
    HAL_TIM_Base_Init(&Buzzer_tim);
    HAL_TIM_PWM_Start(&Buzzer_tim,Buzzer_channel);
}

void Playtone(int num){
    int arr = 0;
    int tone = tones[num-1];
    arr = 36000000.0/tone;
    __HAL_TIM_SET_AUTORELOAD(&Buzzer_tim,arr);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,(int)arr/4);
  }

void PlaytoneArr(int num){
    int arr = tones_arr[num-1];
    __HAL_TIM_SET_AUTORELOAD(&Buzzer_tim,arr);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,(int)arr/4);
  }



  void Stoptone(){
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,1);
  }