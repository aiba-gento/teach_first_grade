#include "can_data_manager.hpp"

CANDataManager *CANDataManager::instance_ = nullptr; // おまじない(chatgpt)

CANDataManager::CANDataManager()
{
    instance_ = this;
}

CANDataManager::~CANDataManager()
{
}

void CANDataManager::init(uint8_t md_id)
{
    targets_packet_offset = md_id % 4 * can_config::dlc::md::targets_1; // 目標値のパケットオフセット
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
    if (flag_md_targets_1) // 目標値の受信
    {
        flag_md_targets_1 = false;                                           // フラグのリセット
        *target = (int16_t)buff_md_targets_1[1] << 8 | buff_md_targets_1[0]; // データの取得
        return true;
    }
    else if (flag_md_targets_4) // 目標値x4の受信
    {
        flag_md_targets_4 = false;                                                                                       // フラグのリセット
        *target = (int16_t)buff_md_targets_4[targets_packet_offset + 1] << 8 | buff_md_targets_4[targets_packet_offset]; // データの取得
        return true;
    }

    return false;
}

bool CANDataManager::getMDMode(md_mode_t *mode)
{
    if (flag_md_mode)
    {
        flag_md_mode = false; // フラグのリセット
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
    if (flag_md_p_gain && flag_md_i_gain && flag_md_d_gain) // 3つのデータが揃った場合
    {
        flag_md_p_gain = false;
        flag_md_i_gain = false;
        flag_md_d_gain = false;
        *p_gain = (float)(buff_md_p_gain[3] << 24 | buff_md_p_gain[2] << 16 | buff_md_p_gain[1] << 8 | buff_md_p_gain[0]); // データの取得
        *i_gain = (float)(buff_md_i_gain[3] << 24 | buff_md_i_gain[2] << 16 | buff_md_i_gain[1] << 8 | buff_md_i_gain[0]);
        *d_gain = (float)(buff_md_d_gain[3] << 24 | buff_md_d_gain[2] << 16 | buff_md_d_gain[1] << 8 | buff_md_d_gain[0]);
        return true;
    }
    return false;
}

void CANDataManager::staticOnReceive(int packetSize)
{
    if (instance_ != nullptr) // インスタンスが存在する場合
    {
        instance_->onReceive(packetSize); // 受信時の処理を実行
    }
    else
    {
        Serial.println("instance is null");
    }
}

void CANDataManager::onReceive(int packetSize)
{
    can_id = CAN.packetId(); // CAN IDの取得
    dlc = CAN.packetDlc();   // Data Length Codeの取得
    for (uint8_t i = 0; i < dlc; i++)
    {
        data[i] = CAN.read(); // データの読み込み
    }
    decodeCanID(can_id, &dir, &dev, &device_id, &data_name);                                                                                         // CAN IDのデコード
    if (dlc == can_config::dlc::md::targets_4 && dir == can_config::dir::to_slave && dev == can_config::dev::motor_driver && device_id == md_id / 4) // モータードライバの目標値x4
    {
        for (uint8_t i = 0; i < dlc; i++)
        {
            buff_md_targets_4[i] = data[i]; // データのコピー
        }
        flag_md_targets_4 = true; // フラグのセット
    }
    else if (dir == can_config::dir::to_slave && dev == can_config::dev::motor_driver && device_id == md_id)
    {
        clessifyData(data_name, data, dlc); // データの分類
    }
}

void CANDataManager::clessifyData(uint8_t data_name, uint8_t *data, uint8_t dlc)
{
    switch (data_name)
    {
    case can_config::data_name::md::init:     // 初期化コマンド
        if (dlc != can_config::dlc::md::init) // データ長が違う場合は無視
            break;
        flag_md_init = true; // フラグのセット
        break;
    case can_config::data_name::md::targets:
        if (dlc == can_config::dlc::md::targets_1) // 目標値
        {
            for (uint8_t i = 0; i < dlc; i++)
            {
                buff_md_targets_1[i] = data[i];
            }
            flag_md_targets_1 = true;
        }
        else
        {
            break; // それ以外は無視
        }
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