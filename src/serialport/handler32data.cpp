//
// Created by zyz on 2023/5/28.
//

#include "handler32data.h"
#include "r32constant.h"

#include <QDebug>

Handler32data::Handler32data(QObject *parent)
    : QObject(parent)
{
    m_readFuncMap = {
            {CMD_ND_01, &Handler32data::readOperateResult},
            {CMD_ND_OVER_02, &Handler32data::readOperateResult},
            {CMD_ND_STATUS_03, &Handler32data::readOperateResult},
            {CMD_READ_R0_04, &Handler32data::readOperateData},
            {CMD_READ_PARAM1_05, &Handler32data::readOperateData},
            {CMD_READ_PARAM2_06, &Handler32data::readOperateData},
            {CMD_READ_1000PPM_07, &Handler32data::readOperateData},
            {CMD_READ_5000PPM_08, &Handler32data::readOperateData},
    };

}

Handler32data::~Handler32data() {
}

void Handler32data::processReceivedData(const QByteArray &data)
{
    m_receivedData.append(data);

    // 1. 判断数据是否有效
    while (m_receivedData.length() >= RECV_FRAME_LENGTH) {
        // 2. 判断帧头是否正确
        if (m_receivedData.at(0) != RECV_HEADER) {
            m_receivedData.remove(0, 1);
            continue;
        }

        // 3.提取完整的一帧数据
        QByteArray frameData = m_receivedData.left(RECV_FRAME_LENGTH);
        m_receivedData.remove(0, RECV_FRAME_LENGTH);

        // 4. 判断校验和是否正确
        if (!frameIsValid(frameData)) {
            qWarning() << "recv frame is valid:" << frameData.toHex();
            continue;
        }

        Q_EMIT recvedFrameData(frameData);

        // 5. 提取命令和数据
        quint8 cmd = static_cast<quint8>(frameData.at(1));
        QByteArray content = frameData.mid(2, 4);

        // 6. 根据命令解析数据
        if (m_readFuncMap.contains(cmd)) {
            QVariantMap value;
            bool ret = (this->*m_readFuncMap.value(cmd))(cmd, content, value);
            if (ret) {
                Q_EMIT frameReceived(cmd, value);
            } else {
                qWarning() << "cmd:"<< cmd << "parse frame failed:" << frameData.toHex();
            }
        } else {
            qWarning() << "unknown command:"<< cmd << "not support:" << frameData.toHex();
        }
    }
}

// 判断帧数据的累加和是否正确
bool Handler32data::frameIsValid(const QByteArray &frameData) {
    int sumLow8 = static_cast<quint8>(frameData.at(frameData.length() - 1));

    int sum = 0;
    for (int i = 0; i < frameData.length() - 2; ++i) {
        sum += frameData.at(i);
    }

    // 取sum的低8字节和sumLow8比较
    if ((sum & 0xFF) != sumLow8) {
        return false;
    }

    return true;
}

bool Handler32data::readOperateResult(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取浓度标定ack
    auto readNdAck = [](const QByteArray &data, QVariantMap &value) {
        quint8 result = static_cast<quint8>(data.at(2));
        value.insert(ACK_RESULT, result == 0 ? "成功" : "失败");
        if (result != 0) {
            quint8 error = static_cast<quint8>(data.at(3));
            switch (error) {
                case 1:
                    value.insert(ACK_ERROR, "浓度超测量量程");
                    break;
                case 2:
                    value.insert(ACK_ERROR, "传感器故障");
                    break;
                case 3:
                    value.insert(ACK_ERROR, "气体探头在加热中");
                    break;
                default:
                    value.insert(ACK_ERROR, "未知错误");
                    break;
            }
        }
    };

    // 读取标定完成ack
    auto readCalibrationAck = [](const QByteArray &data, QVariantMap &value) {
        quint8 result = static_cast<quint8>(data.at(2));
        value.insert(ACK_RESULT, result == 0 ? "成功" : "失败");
        if (result != 0) {
            quint8 error = static_cast<quint8>(data.at(3));
            switch (error) {
                case 1:
                    value.insert(ACK_ERROR, "标定的点数小于二");
                    break;
                case 2:
                    value.insert(ACK_ERROR, "写flash失败");
                    break;
                default:
                    value.insert(ACK_ERROR, "未知错误");
                    break;
            }
        }
    };

    // 查询标定状态ack
    auto readCalibrationStatusAck = [](const QByteArray &data, QVariantMap &value) {
        quint8 result = static_cast<quint8>(data.at(2));
        switch (result) {
            case 0:
                value.insert(ACK_RESULT, "未标定");
                break;
            case 1:
                value.insert(ACK_ERROR, "标定中");
                break;
            case 2:
                value.insert(ACK_ERROR, "标定失败");
                break;
            case 3:
                value.insert(ACK_ERROR, "标定成功");
                break;
            default:
                value.insert(ACK_ERROR, "未知状态");
                break;
        }
    };


    switch (cmd) {
        case CMD_ND_01:
            readNdAck(data, value);
            break;
        case CMD_ND_OVER_02:
            readCalibrationAck(data, value);
            break;
        case CMD_ND_STATUS_03:
            readCalibrationStatusAck(data, value);
            break;
        default:
            qWarning() << "cmd:" << cmd << "not support:" << data.toHex();
            return false;
    }

    return true;
}

bool Handler32data::readOperateData(quint8 cmd, const QByteArray &data, QVariantMap &value) {
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取ack数值
    float fData = 0;
    memcpy(&fData, data.data(), 4);

    switch (cmd) {
        case CMD_READ_R0_04:
            value.insert(ACK_R0_VALUE, fData);
            break;
        case CMD_READ_PARAM1_05:
            value.insert(ACK_PARAM1_VALUE, fData);
            break;
        case CMD_READ_PARAM2_06:
            value.insert(ACK_PARAM2_VALUE, fData);
            break;
        case CMD_READ_1000PPM_07:
            value.insert(ACK_1000PPM_VALUE, fData);
            break;
        case CMD_READ_5000PPM_08:
            value.insert(ACK_5000PPM_VALUE, fData);
            break;
        default:
            qWarning() << "cmd:" << cmd << "not support:" << data.toHex();
            return false;
    }

    return true;
}
