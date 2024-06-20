#pragma once
#include <Arduino.h>
#include "CAN.h"
#include "pin_number.hpp"
#include "can_data_configure.hpp"
#include "htmd_mode.hpp"
#include <memory>

class CANDataManager
{
private:
    // モータードライバのID
    uint8_t md_id;
    uint8_t targets_packet_offset; // 目標値のパケットオフセット
    // CAN通信の設定
    uint32_t can_baudrate = 1000E3; // 1Mbps
    // CAN通信のバッファ
    uint8_t buff_md_init[can_config::dlc::common::init];       // 初期化コマンドのバッファ
    uint8_t buff_md_targets_1[can_config::dlc::md::targets_1]; // 目標値のバッファ
    uint8_t buff_md_targets_4[can_config::dlc::md::targets_4]; // 目標値x4のバッファ
    uint8_t buff_md_mode[can_config::dlc::md::mode];           // モードのバッファ
    uint8_t buff_md_p_gain[can_config::dlc::md::p_gain];       // PID制御の比例ゲインのバッファ
    uint8_t buff_md_i_gain[can_config::dlc::md::i_gain];       // PID制御の積分ゲインのバッファ
    uint8_t buff_md_d_gain[can_config::dlc::md::d_gain];       // PID制御の微分ゲインのバッファ
    // CAN通信のフラグ
    bool flag_md_init;      // 初期化コマンドのフラグ
    bool flag_md_targets_1; // 目標値のフラグ
    bool flag_md_targets_4; // 目標値x4のフラグ
    bool flag_md_mode;      // モードのフラグ
    bool flag_md_p_gain;    // PID制御の比例ゲインのフラグ
    bool flag_md_i_gain;    // PID制御の積分ゲインのフラグ
    bool flag_md_d_gain;    // PID制御の微分ゲインのフラグ
    // 静的コールバック関数
    static void staticOnReceive(int packetSize);
    static CANDataManager *instance;
    // 1時データ
    uint16_t can_id;
    uint8_t dlc;
    uint8_t data[8];
    uint8_t dir;
    uint8_t dev;
    uint8_t device_id;
    uint8_t data_name;

    /**
     * @brief CAN通信の受信
     */
    void onReceive(int packetSize);

    /**
     * @brief CAN通信のデータを分類
     * @param data_name データの種類
     * @param data データ
     * @param dlc Data Length Code
     */
    void clessifyData(uint8_t data_name, uint8_t *data, uint8_t dlc);

public:
    CANDataManager();
    ~CANDataManager();

    /**
     * @brief CAN通信の初期化
     */
    void init(uint8_t md_id);

    /**
     * @brief 初期化コマンドの取得
     * @return bool true: 受信, false: 未受信
     */
    bool getMDInit();

    /**
     * @brief 目標値の取得
     * @param target 目標値
     * @return bool true: 受信, false: 未受信
     */
    bool getMDTargets(int16_t *target);

    /**
     * @brief モードの取得
     * @param mode モード
     * @return bool true: 受信, false: 未受信
     */
    bool getMDMode(md_mode_t *mode);

    /**
     * @brief PID制御の比例ゲインの取得
     * @param p_gain 比例ゲイン
     * @param i_gain 積分ゲイン
     * @param d_gain 微分ゲイン
     * @return bool true: 受信, false: 未受信
     */
    bool getMDPIDGain(float *p_gain, float *i_gain, float *d_gain);
};