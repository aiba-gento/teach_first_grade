#include <Arduino.h>
#include "CAN.h"
#include "can_data_manager.hpp"
#include "Ps3Controller.h"
#include "FastLED.h"

int16_t md_targets[4] = {0};                    // モーターの出力値
CANDataManager can_data_manager;                // CAN通信を管理するクラスのインスタンス生成
CRGB leds[1];                                   // RGBLED
int8_t servo_input[4] = {0, 0, 0, 0};           // サーボの変化量
uint16_t servo_out[4] = {337, 337, 500, 500};   // サーボのパルス幅
uint16_t servo_min[4] = {135, 135, 1500, 1500}; // サーボのパルス幅の最小値
uint16_t servo_max[4] = {540, 540, 2000, 2000}; // サーボのパルス幅の最大値
float servo_gain[4] = {1.0, 1.0, 1.0, 1.0};     // サーボの変化速度のゲイン

/**
 * @brief どんな型の数値であっても値を範囲内に収める
 *
 * @tparam T 整数または少数
 * @param value 入力のポインタ
 * @param min 最小値
 * @param max 最大値
 */
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

/**
 * @brief サーボの角度の変化量をサーボの絶対角に変換する
 *
 * @param servo_1 サーボAの変化量
 * @param servo_2 サーボBの変化量
 * @param servo_3 サーボCの変化量
 * @param servo_4 サーボDの変化量
 */
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
    Serial.begin(115200);                                       // シリアル通信の開始
    can_data_manager.init();                                    // CAN通信の開始
    Ps3.begin("0c:8b:95:2a:c8:80");                             // BluetoothのMACアドレス
    FastLED.addLeds<WS2812, pin_number::RGB_LED, GRB>(leds, 1); // RGBLEDを一つとして初期化
    while (!Ps3.isConnected())                                  // PS3コントローラが接続されるまで黄色を点滅
    {
        leds[0] = CRGB::Yellow;
        FastLED.show();
        delay(500);
        leds[0] = CRGB::Black;
        FastLED.show();
        delay(500);
    }
    can_data_manager.sendYamaHexInit(0); // YamaHexDeviceControllerを初期化（黒い基板のLEDが緑から青になる）

    leds[0] = CRGB::Blue;
    FastLED.show();
}

void loop()
{
    if (Ps3.isConnected()) // PS3コントローラーが接続されているとき
    {
        md_targets[0] = (int16_t)Ps3.data.analog.stick.ly * 20; // 左スティックのY入力に適当な数値をかけてモータAの出力とする。
        md_targets[1] = (int16_t)Ps3.data.analog.stick.ry * 20; // 右スティックのY入力に適当な数値をかけてモータBの出力とする。（マイナスかければ逆回転）
        md_targets[2] = 0;
        md_targets[3] = 0;
        can_data_manager.sendMDTargets_4(0, md_targets);                                   // モーターの出力を送信
        servo_input[0] = (int8_t)Ps3.data.button.up - (int8_t)Ps3.data.button.down;        // サーボAの角度を矢印キーで変化量を計算
        servo_input[1] = (int8_t)Ps3.data.button.triangle - (int8_t)Ps3.data.button.cross; // サーボBの角度を図形キーで変化量を計算
        servo_input[2] = 0;
        servo_input[3] = 0;
        update_servo_angle(servo_input[0], servo_input[1], servo_input[2], servo_input[3]); // 角度の増減をサーボの絶対角に変換
        can_data_manager.sendServoAngles(0, servo_out);                                     // サーボの出力を送信
    }
    else
    {
        leds[0] = CRGB::Red;
        FastLED.show();
    }
}