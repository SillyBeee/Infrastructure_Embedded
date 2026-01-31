#include "encoder.h"
#include "encoder_task.h"

#include "cmsis_os.h"

CanInstance_s* can1;
CanInitConfig_s can1_config = {
    .topic_name = "can1",
    .can_number = 1,
    .tx_id = 0x01,
    .rx_id = 0x01,
  };

EncoderInstance_s* encoder;
EncoderInitConfig_s encoder_config = {
    .topic_name = "encoder1",
    .encoder_address = 0x01,
    .can_config = &can1_config,
    .is_auto_refresh = false,
    .refresh_time = 100,};


void encoder_task(void const * argument){

    encoder = Encoder_Register(&encoder_config);
    if (encoder == NULL) {
        Log_Error("Encoder Register Failed");
    }
    while (1)
    {
        if (encoder!=NULL)
        {
            Encoder_Angle_Refresh(encoder);
        }
        osDelay(1);
    }
}