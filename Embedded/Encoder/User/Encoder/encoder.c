#include "encoder.h"
#include <string.h>
static void Encoder_Decode(CanInstance_s *can_instance) {
   if(can_instance == NULL){
      return;
   }
   EncoderInstance_s *encoder = (EncoderInstance_s *)can_instance->parent_ptr;
   if(encoder == NULL) {
      return;
   }



}


bool Send_Encoder_Refresh_Request(EncoderInstance_s* encoder) {
   if (encoder != NULL && encoder->can_instance != NULL) {
       encoder->can_instance->tx_id = 0x01;
       encoder->can_instance->tx_header.DLC = 4;
       memset(encoder->can_instance->tx_buff,0,sizeof(encoder->can_instance->tx_buff));
       encoder->can_instance->tx_buff[0] = 0x04;
       encoder->can_instance->tx_buff[1] = 0x01;
       encoder->can_instance->tx_buff[2] = 0x01;
       encoder->can_instance->tx_buff[3] = 0x00;
       if (Can_Transmit(encoder->can_instance)) {
          return true;
       }
       return false;
   }
    return false;
    Log_Error("%s : Encoder Send Request Failed", encoder->topic_name);
}

EncoderInstance_s *Encoder_Register(const EncoderInitConfig_s* config) {
   if (config ==NULL) {
      return NULL;
   }
   EncoderInstance_s* instance = user_malloc(sizeof(EncoderInstance_s));
   memset(instance, 0, sizeof(EncoderInstance_s)); // 清空内存
   if (instance == NULL) {
      Log_Error("%s : Encoder Register Failed, No Memory", config->topic_name);
      return NULL;
   }
   instance->topic_name = config->topic_name;
   instance->encoder_address = config->encoder_address;
   instance->is_auto_refresh = config->is_auto_refresh;
   instance->refresh_time = config->refresh_time;

   // 注册编码器到CAN总线
   config->can_config->topic_name = config->topic_name;
   config->can_config->rx_id = 0x01;
   config->can_config->tx_id = 0x01;
   config->can_config->parent_ptr = instance;

   instance->angle = 0;
   return instance;
}

bool Encoder_Angle_Refresh(EncoderInstance_s* instance) {
   if (instance == NULL) {
      Log_Error("Encoder Refresh Failed: Instance NULL");
      return false;
   }


}


