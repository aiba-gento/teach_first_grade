#include "motor_controller.hpp"
#include <Arduino.h>

/**
 * ロジック回路の真理値表
 * | BRAKE | DIR | PWM | H1  | L1  | H2  | L2  |
 * | ----- | --- | --- | --- | --- | --- | --- |
 * | 0     | 0   | 0   | 0   | 0   | 0   | 0   |
 * | 0     | 0   | 1   | 0   | 1   | 1   | 0   |
 * | 0     | 1   | 0   | 0   | 0   | 0   | 0   |
 * | 0     | 1   | 1   | 1   | 0   | 0   | 1   |
 * | 1     | 0   | 0   | 0   | 1   | 0   | 1   |
 * | 1     | 0   | 1   | 0   | 1   | 1   | 0   | <- Don't Care (入力不要)
 * | 1     | 1   | 0   | 0   | 1   | 0   | 1   | <- Don't Care (入力不要)
 * | 1     | 1   | 1   | 1   | 0   | 0   | 1   | <- Don't Care (入力不要)
 */

MotorController::MotorController()
{
}

MotorController::~MotorController()
{
}

void MotorController::init()
{
    // ピンの設定
    pinMode(md_pin::MOTOR_DISABLE, OUTPUT);     // モーターの有効無効
    pinMode(md_pin::MOTOR_BRAKE, OUTPUT);       // ブレーキ
    pinMode(md_pin::MOTOR_DIRECTION, OUTPUT);   // 回転方向
    digitalWrite(md_pin::MOTOR_DISABLE, LOW);   // モーター有効
    digitalWrite(md_pin::MOTOR_BRAKE, LOW);     // ブレーキ無効
    digitalWrite(md_pin::MOTOR_DIRECTION, LOW); // 回転方向
    // PWMの設定
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution); // PWMの設定
    ledcAttachPin(md_pin::MOTOR_PWM, pwm_channel);         // PWMのピン設定
    ledcWrite(pwm_channel, 0);                             // PWMの出力
}

void MotorController::run(int16_t speed)
{
    // モーターの回転方向
    if (speed < 0)
    {
        digitalWrite(md_pin::MOTOR_DIRECTION, LOW);
        speed = -speed;
    }
    else
    {
        digitalWrite(md_pin::MOTOR_DIRECTION, HIGH);
    }
    // モーターの回転速度
    ledcWrite(pwm_channel, speed);
}

void MotorController::disable(bool enable)
{
    if (enable)
    {
        digitalWrite(md_pin::MOTOR_DISABLE, HIGH);
    }
    else
    {
        digitalWrite(md_pin::MOTOR_DISABLE, LOW);
    }
}

void MotorController::brake(bool enable)
{
    if (enable)
    {
        digitalWrite(md_pin::MOTOR_BRAKE, HIGH);
    }
    else
    {
        digitalWrite(md_pin::MOTOR_BRAKE, LOW);
    }
}

uint16_t MotorController::trapezoidal_control(uint16_t input_speed)
{
    // 台形制御
    int16_t speed = input_speed;
    int16_t acceleration = speed - prev_speed; // 加速度
    if (acceleration > max_acceleration)       // 最大加速度を超えた場合
    {
        speed = prev_speed + max_acceleration; // 最大加速度に制限
    }
    else if (acceleration < -max_acceleration) // 最大減速度を超えた場合
    {
        speed = prev_speed - max_acceleration; // 最大減速度に制限
    }
    prev_speed = speed; // 速度の更新
    return speed;
}