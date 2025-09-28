#include "encoder.h"
#include <string.h>
// #include "portable.h"

static void Encoder_Decode(CanInstance_s *can_instance) {
   if(can_instance == NULL){
      return;
   }
   EncoderInstance_s *encoder = (EncoderInstance_s *)can_instance->parent_ptr;
   if(encoder == NULL) {
      return;
   }
   const int length = can_instance->rx_buff[0];
   if (length!= 0x07) {
      //如果长度不为7,则不是角度返回值
      return;
   }
   int address = can_instance->rx_buff[1];
   if (address != encoder->encoder_address) {
      return;
   }
   int function = can_instance->rx_buff[2];
   uint32_t data =
      can_instance->rx_buff[6]<<24 |
      can_instance->rx_buff[5]<<16 |
      can_instance->rx_buff[4] <<8 |
      can_instance->rx_buff[3];
   encoder->angle = data*360/1024.0;

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
   EncoderInstance_s* encoder_instance = user_malloc(sizeof(EncoderInstance_s));
   memset(encoder_instance, 0, sizeof(EncoderInstance_s)); // 清空内存
   if (encoder_instance == NULL) {
      Log_Error("%s : Encoder Register Failed, No Memory", config->topic_name);
      return NULL;
   }
   encoder_instance->topic_name = config->topic_name;
   encoder_instance->encoder_address = config->encoder_address;
   encoder_instance->is_auto_refresh = config->is_auto_refresh;
   encoder_instance->refresh_time = config->refresh_time;

   // 注册编码器到CAN总线
   config->can_config->topic_name = config->topic_name;
   config->can_config->rx_id = 0x01;
   config->can_config->tx_id = 0x01;
   config->can_config->parent_ptr = encoder_instance;
   config->can_config->can_module_callback = Encoder_Decode;
   encoder_instance->can_instance = Can_Register(config->can_config);
   if (encoder_instance->can_instance == NULL) {
      user_free(encoder_instance);
      return NULL;
   }

   encoder_instance->angle = 0;
   return encoder_instance;
}

bool Encoder_Angle_Refresh(EncoderInstance_s* instance) {
   if (instance == NULL) {
      Log_Error("Encoder Refresh Failed: Instance NULL");
      return false;
   }
   Send_Encoder_Refresh_Request(instance);


}


