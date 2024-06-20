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

namespace pin_number
{
    static constexpr uint8_t CAN_TX = 33;
    static constexpr uint8_t CAN_RX = 32;
    static constexpr uint8_t SPI_MISO = 19;
    static constexpr uint8_t SPI_MOSI = 23;
    static constexpr uint8_t SPI_SCK = 18;
    static constexpr uint8_t SPI_SS1 = 25;
    static constexpr uint8_t SPI_SS2 = 26;
    static constexpr uint8_t LIMIT_SW1 = 34;
    static constexpr uint8_t LIMIT_SW2 = 35;
    static constexpr uint8_t RGB_LED = 14;
    static constexpr uint8_t PILOT_LAMP = 27;
}