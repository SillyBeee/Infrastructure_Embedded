#include "led.h"

void led_task(void const *argument)
{
    while (1)
    {
       RGB_LED();
    }
}