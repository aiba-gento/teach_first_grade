#pragma once
#include "CAN.h"
#include "can_data_configure.hpp"
#include "htmd_mode.hpp"
#include "pin_number.hpp"

class CANDataManager
{
private:
    // CAN通信の設定
    uint32_t can_baudrate = 1000E3; // 1Mbps
    // 静的コールバック関数
    static void staticOnReceive(int packetSize);
    static CANDataManager *instance_;
    // 一時データ
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
    void init();

    void sendMDInit(uint8_t md_id);
    void sendYamaHexInit(uint8_t id);
    void sendMDMode(uint8_t md_id, md_mode_t mode);
    void sendMDTargets_1(uint8_t md_id, uint16_t target_1);
    void sendMDTargets_4(uint8_t md_id, uint16_t *targets);
    void sendServoAngles(uint8_t servo_id, uint16_t angle[4]);
};