#include "can_data_manager.hpp"

CANDataManager::CANDataManager()
{
}

CANDataManager::~CANDataManager()
{
}

void CANDataManager::init(uint8_t md_id)
{
    targets_packet_offset = md_id % 4 * can_config::dlc::md::targets_1; // 目標値のパケットオフセット
    instance = this;                                                    // インスタンスの設定
    this->md_id = md_id;                                                // モータードライバのID
    // CAN通信の初期化
    CAN.setPins(md_pin::CAN_RX, md_pin::CAN_TX); // CAN通信のピン設定
    CAN.begin(can_baudrate);                     // CAN通信の開始
    CAN.onReceive(staticOnReceive);              // 受信時のコールバック関数
}

bool CANDataManager::getMDInit()
{
    if (flag_md_init)
    {
        flag_md_init = false;
        return true;
    }
    return false;
}

bool CANDataManager::getMDTargets(int16_t *target)
{
    if (flag_md_targets_1)
    {
        flag_md_targets_1 = false;
        *target = (int16_t)buff_md_targets_1[1] << 8 | buff_md_targets_1[0];
        return true;
    }
    else if (flag_md_targets_4)
    {
        flag_md_targets_4 = false;
        *target = (int16_t)buff_md_targets_4[targets_packet_offset + 1] << 8 | buff_md_targets_4[targets_packet_offset];
        return true;
    }

    return false;
}

bool CANDataManager::getMDMode(md_mode_t *mode)
{
    if (flag_md_mode)
    {
        flag_md_mode = false;
        for (uint8_t i = 0; i < can_config::dlc::md::mode; i++)
        {
            mode->code[i] = buff_md_mode[i];
        }
        return true;
    }
    return false;
}

bool CANDataManager::getMDPIDGain(float *p_gain, float *i_gain, float *d_gain)
{
    if (flag_md_p_gain && flag_md_i_gain && flag_md_d_gain)
    {
        flag_md_p_gain = false;
        flag_md_i_gain = false;
        flag_md_d_gain = false;
        *p_gain = (float)(buff_md_p_gain[3] << 24 | buff_md_p_gain[2] << 16 | buff_md_p_gain[1] << 8 | buff_md_p_gain[0]);
        *i_gain = (float)(buff_md_i_gain[3] << 24 | buff_md_i_gain[2] << 16 | buff_md_i_gain[1] << 8 | buff_md_i_gain[0]);
        *d_gain = (float)(buff_md_d_gain[3] << 24 | buff_md_d_gain[2] << 16 | buff_md_d_gain[1] << 8 | buff_md_d_gain[0]);
        return true;
    }
    return false;
}

void CANDataManager::staticOnReceive(int packetSize)
{
    instance->onReceive(packetSize);
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
    if (dev == can_config::dev::motor_driver && device_id == md_id)
    {
        clessifyData(data_name, data, dlc);
    }
}

void CANDataManager::clessifyData(uint8_t data_name, uint8_t *data, uint8_t dlc)
{
    switch (data_name)
    {
    case can_config::data_name::md::init:
        if (dlc != can_config::dlc::md::init)
            break;
        flag_md_init = true;
        break;
    case can_config::data_name::md::targets:
        if (can_config::data_name::md::targets == can_config::dlc::md::targets_1)
        {
            for (uint8_t i = 0; i < dlc; i++)
            {
                buff_md_targets_1[i] = data[i];
            }
        }
        else if (can_config::data_name::md::targets == can_config::dlc::md::targets_4)
        {
            for (uint8_t i = 0; i < dlc; i++)
            {
                buff_md_targets_4[i] = data[i];
            }
        }
        else
        {
            break;
        }
        flag_md_targets_1 = true;
        break;
    case can_config::data_name::md::mode:
        if (dlc != can_config::dlc::md::mode)
            break;
        for (uint8_t i = 0; i < dlc; i++)
        {
            buff_md_mode[i] = data[i];
        }
        flag_md_mode = true;
        break;
    case can_config::data_name::md::p_gain:
        if (dlc != can_config::dlc::md::p_gain)
            break;
        for (uint8_t i = 0; i < dlc; i++)
        {
            buff_md_p_gain[i] = data[i];
        }
        flag_md_p_gain = true;
        break;
    case can_config::data_name::md::i_gain:
        if (dlc != can_config::dlc::md::i_gain)
            break;
        for (uint8_t i = 0; i < dlc; i++)
        {
            buff_md_i_gain[i] = data[i];
        }
        flag_md_i_gain = true;
        break;
    case can_config::data_name::md::d_gain:
        if (dlc != can_config::dlc::md::d_gain)
            break;
        for (uint8_t i = 0; i < dlc; i++)
        {
            buff_md_d_gain[i] = data[i];
        }
        flag_md_d_gain = true;
        break;
    default:
        break;
    }
}