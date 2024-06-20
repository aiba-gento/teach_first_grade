#pragma once
#include "pin_number.hpp"
#include <stdbool.h>

class MotorController
{
private:
    // モーターの設定
    int16_t max_duty = 2047; // モーターの最大出力
    // PWMの設定
    uint32_t pwm_frequency = 20000; // 20kHz
    uint8_t pwm_resolution = 11;    // 11bit
    uint8_t pwm_channel = 0;        // PWMチャンネル
    // 台形制御の設定
    int16_t max_acceleration = 100; // 最大加速度
    int16_t prev_speed = 0;         // 前回の速度
public:
    MotorController();
    ~MotorController();

    /**
     * @brief モーターの初期化
     * @note モーターを回す前にsetup関数で呼び出す
     */
    void init();

    /**
     * @brief モーターを回す
     * @param speed 回転速度
     */
    void run(int16_t speed);

    /**
     * @brief モーターを出力を無効にする
     * @param enable true: 無効にする, false: 有効にする
     */
    void disable(bool enable);

    /**
     * @brief モーターをブレーキする
     * @param enable true: ブレーキをかける, false: ブレーキを解除する
     */
    void brake(bool enable);

    /**
     * @brief 台形制御を行う
     *
     * @param input_speed 台形制御前の速度
     * @return uint16_t 台形制御後の速度
     */
    uint16_t trapezoidal_control(uint16_t input_speed);
};