#include "tim.h"

#define Buzzer_tim htim4
#define Buzzer_channel TIM_CHANNEL_3


uint16_t buzzerval = 0;
int tones[12] = {261,270,293,311,329,349,370,392,415,440,466,493};
// int tones_arr[12] = {137931,133333,122866,115755,109422,103151,97297,91836,86746,81818,77253,73022};
int tones_arr[12] = {45977,44444,40955,38585,36474,34383,32432,30612,28915,27272,25751,24340};


void Playtone(int num);

void PlaytoneArr(int num);

void Stoptone();