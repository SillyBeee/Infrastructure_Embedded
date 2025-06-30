#ifndef MOTOR_STEPPER_H
#define MOTOR_STEPPER_H
#include "motor.h"
#include <array>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


class ZDT_Stepper : public Motor
{
public:
    explicit ZDT_Stepper(Motor_Mode mode = NONE_MODE, uint8_t id = 1);
    void Set_Angle(float angle, float speed) override;
    void Set_Speed(float speed) override;

    void Enable();
    void Disable();

    void Set_Mode(Motor_Mode mode);
    void Set_ID(uint8_t id);

private:
    static void ZDT_Error_Handler();
    void Pid_Update(float target) override;


    uint8_t tx_data[16];
    Motor_Mode mode;
    uint8_t id;
};


inline std::array<uint8_t, 2> DecimalToHexArray(uint16_t decimal)
{
    std::array<uint8_t, 2> hex_array = {0};
    hex_array[0] = (decimal >> 8) & 0xFF; // 高8位
    hex_array[1] = decimal & 0xFF; // 低8位
    return hex_array;
}

#endif //MOTOR_STEPPER_H
