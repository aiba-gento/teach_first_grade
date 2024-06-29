#include <Arduino.h>
#include "can_data_configure.hpp"
#include "CAN.h"

uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::solenoid_driver, 0, can_config::data_name::solenoid::targets);
uint8_t rx_data = 0;

void onReceive(int packet_size)
{
    uint16_t id = CAN.packetId();
    uint8_t len = CAN.packetDlc();
    uint8_t data = CAN.read();
    if (id == can_id)
    {
        rx_data = data;
    }
}

void setup()
{
    Serial.begin(115200);
    CAN.setPins(4, 5);
    CAN.begin(1000E3);
    CAN.onReceive(onReceive);
    Serial.println("CAN init");
}

void loop()
{
    digitalWrite(16, rx_data);
    digitalWrite(17, rx_data);
}