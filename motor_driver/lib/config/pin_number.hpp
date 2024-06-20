/**
 * @file PinNumber.h
 * @author gn10
 * @brief ピン番号の定義
 * @version 0.1
 * @date 2024-06-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdint.h>

namespace md_pin
{
    static constexpr uint8_t CAN_TX = 27;
    static constexpr uint8_t CAN_RX = 33;
    static constexpr uint8_t MOTOR_DISABLE = 25;
    static constexpr uint8_t MOTOR_BRAKE = 26;
    static constexpr uint8_t MOTOR_DIRECTION = 22;
    static constexpr uint8_t MOTOR_PWM = 21;
    static constexpr uint8_t ENCODER_A = 23;
    static constexpr uint8_t ENCODER_B = 19;
    static constexpr uint8_t ENCODER_MISO = 19;
    static constexpr uint8_t ENCODER_MOSI = 23;
    static constexpr uint8_t ENCODER_SCK = 18;
    static constexpr uint8_t ENCODER_SS = 15;
    static constexpr uint8_t LIMIT_SW1 = 17;
    static constexpr uint8_t LIMIT_SW2 = 16;
}