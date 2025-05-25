#ifndef MOTOR_GM_H
#define MOTOR_GM_H
// #include "cancan.h"
#include "motor.h"
#define GM6020_MAX_CURRENT (16384)
#define GM6020_MAX_VOLTAGE (25000)
#define GM6020_MAX_POSITION (8191)
#define GM6020_STDID_START_ADDRESS (0x204)
#define GM6020_VOLTAGE_LOW_STDID (0x1FF)
#define GM6020_VOLTAGE_HIGH_STDID (0x2FF)
#define GM6020_CURRENT_LOW_STDID (0x1FE)
#define GM6020_CURRENT_HIGH_STDID (0x2FE)
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
}
#endif

class GM6020 final : private Motor
{
public:
    explicit GM6020(Motor_GM_Mode mode = CURRENT_CTL , CAN_HandleTypeDef* hcan = &hcan1, uint8_t id = 1, Pid_Type pid_type = NONE_PID);
    void Set_Angle(float angle , float speed ) override;
    void Set_Speed(float speed) override;
    void Set_Pid_Type(Pid_Type pid_type) override;
    void Set_ID(uint8_t id);
    
    void Deserialize_Status(const uint8_t* status_buffer);

    void Bind_CAN(CAN_HandleTypeDef* hcan);
    void Set_CTL_Mode(Motor_GM_Mode mode);
    uint8_t Get_ID() const;
    friend void GM_Msg_Send( GM6020* motors , int motor_num ,CAN_HandleTypeDef* hcan);
    PID pid_speed ;
    PID pid_angle ;
    uint8_t status_buffer[8] = {0};
    GM_Motor_Status status;
private:
    static void GM_Error_Handler();
    void Pid_Update(float target) override;



    Motor_GM_Mode mode;
    uint8_t id;
    //继承来的私有变量
    Pid_Type pid_type;
    CAN_HandleTypeDef* hcan = nullptr;

};

inline void Send_ElectricCurrent_CTL_Message_Low(CAN_HandleTypeDef* hcan, int16_t var1, int16_t var2, int16_t var3, int16_t var4);
inline void Send_Voltage_CTL_Message_Low(CAN_HandleTypeDef* hcan, int16_t var1, int16_t var2, int16_t var3, int16_t var4);
inline void Send_ElectricCurrent_CTL_Message_High(CAN_HandleTypeDef* hcan, int16_t var1, int16_t var2, int16_t var3);
inline void Send_Voltage_CTL_Message_High(CAN_HandleTypeDef* hcan, int16_t var1, int16_t var2, int16_t var3);


inline void GM_Msg_Send( GM6020* motors , int motor_num , CAN_HandleTypeDef* hcan)
{
    if (motor_num == 0)
    {
        return;
    }
    int16_t low_outputs[4] = {0}, high_outputs[3] = {0};
    int low_count = 0, high_count = 0;

    for (int i = 0; i < motor_num; ++i) {
        uint8_t id = motors[i].id;
        auto pid_output =static_cast<int16_t>(motors[i].pid_speed.Get_Output());
        if (pid_output == 0)
        {
            HAL_GPIO_WritePin(GPIOH , GPIO_PIN_10,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOH , GPIO_PIN_11,GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOH , GPIO_PIN_12,GPIO_PIN_RESET);
        }
        //错误处理
        if (motors[i].hcan != hcan){
            motors[i].GM_Error_Handler();
            return;
        }
        if (id >= 1 && id <= 4 && low_count <= 4) {
            low_outputs[id-1] = pid_output;
            low_count++;
        } else if (id >= 5 && id <= 7 && high_count <= 3) {
            high_outputs[id-5] = pid_output;
            high_count++;
        }
    }

    // 发送低 ID 的电机控制消息
    if (low_count > 0) {
        if (motors[0].mode == CURRENT_CTL) {
            Send_ElectricCurrent_CTL_Message_Low(hcan,
                low_outputs[0], low_outputs[1], low_outputs[2], low_outputs[3]);
        } else if (motors[0].mode == VOLTAGE_CTL) {
            Send_Voltage_CTL_Message_Low(hcan,
                low_outputs[0], low_outputs[1], low_outputs[2], low_outputs[3]);
        }
    }

    // 发送高 ID 的电机控制消息
    if (high_count > 0) {
        if (motors[0].mode == CURRENT_CTL) {
            Send_ElectricCurrent_CTL_Message_High(hcan,
                high_outputs[0], high_outputs[1], high_outputs[2]);
        } else if (motors[0].mode == VOLTAGE_CTL) {
            Send_Voltage_CTL_Message_High(hcan,
                high_outputs[0], high_outputs[1], high_outputs[2]);
        }
    }
}







inline void Send_Voltage_CTL_Message_Low(CAN_HandleTypeDef* hcan, const int16_t var1 , const int16_t var2 , const int16_t var3 , const int16_t var4)
{

    //错误处理
    if (var1 > GM6020_MAX_VOLTAGE || var1 < -GM6020_MAX_VOLTAGE ||
        var2 > GM6020_MAX_VOLTAGE || var2 < -GM6020_MAX_VOLTAGE ||
        var3 > GM6020_MAX_VOLTAGE || var3 < -GM6020_MAX_VOLTAGE ||
        var4 > GM6020_MAX_VOLTAGE || var4 < -GM6020_MAX_VOLTAGE){
        return;
    }
    //发送基本配置
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = GM6020_VOLTAGE_LOW_STDID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    //发送数据帧解析
    uint8_t tx_data[8];
    uint32_t tx_mailbox;
    tx_data[0] = var1 >> 8;
    tx_data[1] = var1 & 0xFF;
    tx_data[2] = var2 >> 8;
    tx_data[3] = var2 & 0xFF;
    tx_data[4] = var3 >> 8;
    tx_data[5] = var3 & 0xFF;
    tx_data[6] = var4 >> 8;
    tx_data[7] = var4 & 0xFF;

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK){
// 		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10 , GPIO_PIN_SET);
// //		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10 , GPIO_PIN_RESET);
	}


}

inline void Send_Voltage_CTL_Message_High(CAN_HandleTypeDef* hcan,int16_t var1 , int16_t var2 , int16_t var3 )
{
    //错误处理
    if (var1 > GM6020_MAX_VOLTAGE || var1 < -GM6020_MAX_VOLTAGE ||
        var2 > GM6020_MAX_VOLTAGE || var2 < -GM6020_MAX_VOLTAGE ||
        var3 > GM6020_MAX_VOLTAGE || var3 < -GM6020_MAX_VOLTAGE){
        return;
    }
    //发送格式配置
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = GM6020_VOLTAGE_HIGH_STDID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    //发送数据帧解析
    uint8_t tx_data[8];
    uint32_t tx_mailbox;
    tx_data[0] = var1 >> 8;
    tx_data[1] = var1 & 0xFF;
    tx_data[2] = var2 >> 8;
    tx_data[3] = var2 & 0xFF;
    tx_data[4] = var3>> 8;
    tx_data[5] = var3 & 0xFF;
    tx_data[6] = 0; // 填充NULL数据
    tx_data[7] = 0; // 填充NULL数据

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK){}
}

inline void Send_ElectricCurrent_CTL_Message_Low(CAN_HandleTypeDef* hcan,int16_t var1 , int16_t var2 , int16_t var3 ,int16_t var4)
{
    //错误处理
    if (var1 > GM6020_MAX_CURRENT || var1 < -GM6020_MAX_CURRENT ||
        var2 > GM6020_MAX_CURRENT || var2 < -GM6020_MAX_CURRENT ||
        var3 > GM6020_MAX_CURRENT || var3 < -GM6020_MAX_CURRENT ||
        var4 > GM6020_MAX_CURRENT || var4 < -GM6020_MAX_CURRENT){
        return;
    }
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = GM6020_CURRENT_LOW_STDID;
    // tx_header.StdId = 0x1FE;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    //发送数据帧解析
    uint8_t tx_data[8];
    uint32_t tx_mailbox;
    tx_data[0] = var1 >> 8;
    tx_data[1] =var1 & 0xFF;
    tx_data[2] = var2 >> 8;
    tx_data[3] = var2 & 0xFF;
    tx_data[4] =var3>> 8;
    tx_data[5] = var3 & 0xFF;
    tx_data[6] = var4 >> 8;
    tx_data[7] = var4 & 0xFF;

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK)
    {
        HAL_GPIO_WritePin(GPIOH , GPIO_PIN_10,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH , GPIO_PIN_11,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOH , GPIO_PIN_12,GPIO_PIN_RESET);
    }
}


inline void Send_ElectricCurrent_CTL_Message_High(CAN_HandleTypeDef* hcan,int16_t var1 , int16_t var2 , int16_t var3 )
{
    //错误处理
    if (var1 > GM6020_MAX_CURRENT || var1 < -GM6020_MAX_CURRENT ||
        var2 > GM6020_MAX_CURRENT || var2 < -GM6020_MAX_CURRENT ||
        var3 > GM6020_MAX_CURRENT || var3 < -GM6020_MAX_CURRENT ){
        return;
    }
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = GM6020_CURRENT_HIGH_STDID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    //发送数据帧解析
    uint8_t tx_data[8];
    uint32_t tx_mailbox;
    tx_data[0] = var1 >> 8;
    tx_data[1] = var1 & 0xFF;
    tx_data[2] = var2 >> 8;
    tx_data[3] = var2 & 0xFF;
    tx_data[4] = var3>> 8;
    tx_data[5] = var3 & 0xFF;
    tx_data[6] = 0; // 填充NULL数据
    tx_data[7] = 0; // 填充NULL数据


    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK){}
}

#endif //MOTOR_GM_H
