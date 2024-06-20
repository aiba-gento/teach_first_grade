#include <Arduino.h>
#include "CAN.h"
#include "can_data_manager.hpp"
#include "Ps3Controller.h"
#include "FastLED.h"

static constexpr uint8_t MD_NUM = 2;
int16_t md_targets[MD_NUM] = {0};
CANDataManager can_data_manager;
CRGB leds[1];

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
        md_targets[0] = (int16_t)Ps3.data.analog.stick.ly * 7;
        md_targets[1] = (int16_t)Ps3.data.analog.stick.ry * 7;
        for (uint8_t i = 0; i < MD_NUM; i++)
        {
            can_data_manager.sendMDTargets_1(i, md_targets[i]);
        }
    }
    else
    {
        leds[0] = CRGB::Red;
        FastLED.show();
    }
}