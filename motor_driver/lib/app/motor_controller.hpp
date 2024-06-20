#pragma once
#include "pin_number.hpp"
#include "htmd_mode.hpp"
#include <stdbool.h>

class MotorController
{
private:
    // モーターの設定
    int16_t max_duty = 2047;    // モーターの最大出力
    uint8_t control_cycle = 10; // 制御周期[ms]
    // PWMの設定
    uint32_t pwm_frequency = 20000; // 20kHz
    uint8_t pwm_resolution = 11;    // 11bit
    uint8_t pwm_channel = 0;        // PWMチャンネル
    // 台形制御の設定
    int16_t max_acceleration = 100; // 最大加速度
    int16_t prev_speed = 0;         // 前回の速度
    // PID制御の設定
    float p_gain = 0.0; // 比例ゲイン
    float i_gain = 0.0; // 積分ゲイン
    float d_gain = 0.0; // 微分ゲイン

    /**
     * @brief 台形制御を行う
     *
     * @param input_speed 台形制御前の速度
     * @return uint16_t 台形制御後の速度
     */
    uint16_t trapezoidal_control(uint16_t input_speed);

    /**
     * @brief モーターをブレーキする
     * @param enable true: ブレーキをかける, false: ブレーキを解除する
     */
    void brake(bool enable);

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
     * @brief モーターのパラメータを設定する
     * @param mode モード
     */
    void setParameters(md_mode_t mode);

    /**
     * @brief モーターを出力を無効にする
     * @param enable true: 無効にする, false: 有効にする
     */
    void disable(bool enable);
};