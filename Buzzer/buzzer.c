#include "buzzer.h"

void playtone(int num){
    int arr = 0;
    int tone = tones[num-1];
    arr = 36000000.0/tone;
    __HAL_TIM_SET_AUTORELOAD(&htim4,arr);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,(int)arr/4);
  }

void playtone_arr(int num){
    int arr = tones_arr[num-1];
    __HAL_TIM_SET_AUTORELOAD(&htim4,arr);
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,(int)arr/4);
  }



  void stoptone(){
    __HAL_TIM_SET_COMPARE(&Buzzer_tim,Buzzer_channel,1);
  }