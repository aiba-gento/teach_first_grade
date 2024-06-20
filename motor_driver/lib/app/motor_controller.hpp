#pragma once
#include "pin_number.h"
#include <stdbool.h>

class MotorController
{
private:
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