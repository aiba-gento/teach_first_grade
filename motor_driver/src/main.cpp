#include <Arduino.h>
#include "motor_controller.hpp"
#include "can_data_manager.hpp"

uint8_t md_id = 1;
bool initialized = false;
int16_t target_speed = 0;
MotorController motor_controller;
CANDataManager can_data_manager;
md_mode_t md_mode;

void setup()
{
    motor_controller.init();        // モーターの初期化
    can_data_manager.init(md_id);   // CAN通信の初期化
    motor_controller.run(0);        // モーターを停止
    motor_controller.disable(true); // モーターを無効化
}

void loop()
{
    if (initialized) // 初期化が完了したら
    {
        if (can_data_manager.getMDTargets(&target_speed)) // 目標値の取得
        {
        }
        if (can_data_manager.getMDMode(&md_mode)) // モードの取得
        {
            motor_controller.setParameters(md_mode); // モードの設定
        }
        motor_controller.run(target_speed); // モーターを回す
    }
    else // 初期化が完了していない場合
    {
        if (can_data_manager.getMDInit()) // 初期化コマンドの取得
        {
            motor_controller.disable(false); // モーターを有効化
            initialized = true;              // 初期化完了
        }
    }
}