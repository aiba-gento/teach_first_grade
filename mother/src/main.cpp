#include <Arduino.h>
#include "CAN.h"
#include "can_data_manager.hpp"
#include "Ps3Controller.h"
#include "FastLED.h"

static constexpr uint8_t MD_NUM = 2;
int16_t md_targets[MD_NUM] = {0};
CANDataManager can_data_manager;
CRGB leds[1];
int8_t servo_input[4] = {0, 0, 0, 0};
uint16_t servo_out[4] = {1500, 1500, 1500, 1500};
uint16_t servo_min[4] = {500, 500, 500, 500};
uint16_t servo_max[4] = {2500, 2500, 2500, 2500};
float servo_gain[4] = {1.0, 1.0, 1.0, 1.0};

template <typename T>
void saturate_value(T *value, T min, T max)
{
    if (*value < min)
    {
        *value = min;
    }
    else if (*value > max)
    {
        *value = max;
    }
}

void update_servo_angle(int8_t servo_1, int8_t servo_2, int8_t servo_3, int8_t servo_4)
{
    servo_out[0] += servo_gain[0] * servo_1;
    servo_out[1] += servo_gain[1] * servo_2;
    servo_out[2] += servo_gain[2] * servo_3;
    servo_out[3] += servo_gain[3] * servo_4;
    for (uint8_t i = 0; i < 4; i++)
    {
        saturate_value(&servo_out[i], servo_min[i], servo_max[i]);
    }
    Serial.printf("servo_out: %d, %d, %d, %d\n", servo_out[0], servo_out[1], servo_out[2], servo_out[3]);
}

void setup()
{
    can_data_manager.init();
    Ps3.begin("0c:8b:95:2a:c1:94");
    FastLED.addLeds<WS2812, pin_number::RGB_LED, GRB>(leds, 1);
    while (!Ps3.isConnected())
    {
        leds[0] = CRGB::Yellow;
        FastLED.show();
        delay(500);
        leds[0] = CRGB::Black;
        FastLED.show();
        delay(500);
    }
    for (uint8_t i = 0; i < MD_NUM; i++)
    {
        can_data_manager.sendMDInit(i);
    }

    leds[0] = CRGB::Blue;
    FastLED.show();
}

void loop()
{
    if (Ps3.isConnected())
    {
        md_targets[0] = (int16_t)Ps3.data.analog.stick.ly * 20;
        md_targets[1] = (int16_t)Ps3.data.analog.stick.ry * 20;
        for (uint8_t i = 0; i < MD_NUM; i++)
        {
            can_data_manager.sendMDTargets_1(i, md_targets[i]);
        }
        servo_input[0] = (int8_t)Ps3.data.button.up - (int8_t)Ps3.data.button.down;
        servo_input[1] = (int8_t)Ps3.data.button.triangle - (int8_t)Ps3.data.button.cross;
        servo_input[2] = 0;
        servo_input[3] = 0;
        update_servo_angle(servo_input[0], servo_input[1], servo_input[2], servo_input[3]);
        can_data_manager.sendServoAngles(0, servo_out);
    }
    else
    {
        leds[0] = CRGB::Red;
        FastLED.show();
    }
}