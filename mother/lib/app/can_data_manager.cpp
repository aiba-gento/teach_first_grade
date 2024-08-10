#include "can_data_manager.hpp"

CANDataManager *CANDataManager::instance_ = nullptr; // おまじない(chatgpt)

CANDataManager::CANDataManager()
{
    instance_ = this;
}

CANDataManager::~CANDataManager()
{
}

void CANDataManager::init()
{
    CAN.setPins(pin_number::CAN_RX, pin_number::CAN_TX); // CAN通信のピン設定
    CAN.begin(can_baudrate);                             // CAN通信の開始
    CAN.onReceive(staticOnReceive);                      // 受信時のコールバック関数
}

void CANDataManager::staticOnReceive(int packetSize)
{
    instance_->onReceive(packetSize);
}

void CANDataManager::onReceive(int packetSize)
{
    can_id = CAN.packetId();
    dlc = CAN.packetDlc();
    for (uint8_t i = 0; i < dlc; i++)
    {
        data[i] = CAN.read();
    }
    decodeCanID(can_id, &dir, &dev, &device_id, &data_name);
    if (dir == can_config::dir::to_master)
    {
        clessifyData(data_name, data, dlc);
    }
}

void CANDataManager::sendMDInit(uint8_t md_id)
{
    uint8_t data[can_config::dlc::md::init] = {0};
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::motor_driver, md_id, can_config::data_name::md::init);
    CAN.beginPacket(can_id);
    CAN.write(data[0]);
    CAN.endPacket();
}

void CANDataManager::sendYamaHexInit(uint8_t id)
{
    uint8_t data[can_config::dlc::common::init] = {0};
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::common, id, can_config::data_name::common::init);
    CAN.beginPacket(can_id);
    CAN.write(data[0]);
    CAN.endPacket();
}

void CANDataManager::sendMDMode(uint8_t md_id, md_mode_t mode)
{
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::motor_driver, md_id, can_config::data_name::md::mode);
    CAN.beginPacket(can_id);
    for (uint8_t i = 0; i < can_config::dlc::md::mode; i++)
    {
        CAN.write(mode.code[i]);
    }
    CAN.endPacket();
}

void CANDataManager::sendMDTargets_1(uint8_t md_id, int16_t target_1)
{
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::motor_driver, md_id, can_config::data_name::md::targets);
    CAN.beginPacket(can_id);
    for (uint8_t i = 0; i < can_config::dlc::md::targets_1; i++)
    {
        CAN.write((uint8_t)(target_1 >> (8 * i)));
    }
    CAN.endPacket();
}

void CANDataManager::sendMDTargets_4(uint8_t md_id, int16_t targets[4])
{
    uint16_t tx_data[4] = {0};
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::motor_driver, md_id, can_config::data_name::md::targets);
    CAN.beginPacket(can_id);
    for (uint8_t i = 0; i < can_config::dlc::md::targets_4 / can_config::dlc::md::targets_1; i++)
    {
        tx_data[i] = static_cast<uint16_t>(targets[i]);
        for (uint8_t j = 0; j < can_config::dlc::md::targets_1; j++)
        {
            CAN.write((uint8_t)(tx_data[i] >> (8 * j)));
        }
    }
    CAN.endPacket();
}

void CANDataManager::sendServoAngles(uint8_t servo_id, uint16_t angle[4])
{
    uint16_t can_id = encodeCanID(can_config::dir::to_slave, can_config::dev::servo_driver, servo_id, can_config::data_name::servo::targets_1_4);
    CAN.beginPacket(can_id);
    for (uint8_t j = 0; j < 4; j++)
    {
        for (uint8_t i = 0; i < 2; i++)
        {
            CAN.write((uint8_t)(angle[j] >> (8 * i)));
        }
    }
    CAN.endPacket();
}

void CANDataManager::clessifyData(uint8_t data_name, uint8_t *data, uint8_t dlc)
{
    switch (data_name)
    {
    case can_config::data_name::md::init:
        break;
    case can_config::data_name::md::mode:
        break;
    case can_config::data_name::md::targets:
        break;
    default:
        break;
    }
}