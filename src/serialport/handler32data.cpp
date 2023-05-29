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
            {CMD_READ_TEMP_HUM_21, &Handler32data::readTemperatureHumidity},
            {CMD_READ_ADC_22, &Handler32data::readGasProbeADC},
            {CMD_READ_RESISTANCE_23, &Handler32data::readGasProbeResistance},
            {CMD_READ_COMPENSATION_RESISTANCE_24, &Handler32data::readGasProbeResistance},
            {CMD_READ_GAS_CONCENTRATION_25, &Handler32data::readGasConcentration},
            {CMD_READ_FIRMWARE_VERSION_41, &Handler32data::readFirmwareVersion},
            {CMD_OPEN_CLOSE_PRINT_42, &Handler32data::readOperateResult},
            {CMD_QUERY_FAULT_STATUS_43, &Handler32data::readProductFaultStatus},
            {CMD_SET_ALARM_THRESHOLD_44, &Handler32data::readOperateResult},
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

    // 打开或者关闭周期性打印数据
    auto readPrintDataAck = [](const QByteArray &data, QVariantMap &value) {
        quint8 result = static_cast<quint8>(data.at(2));
        value.insert(ACK_RESULT, result == 0 ? "成功" : "失败");
        if (result != 0) {
            quint8 error = static_cast<quint8>(data.at(3));
            switch (error) {
                case 0:
                    value.insert(ACK_ERROR, "无错误");
                    break;
                case 1:
                    value.insert(ACK_ERROR, "数据超范围");
                    break;
                default:
                    value.insert(ACK_ERROR, "未知错误");
                    break;
            }
        }
    };

    // 设置报警阈值设置报警阈值
    auto readAlarmThresholdAck = [](const QByteArray &data, QVariantMap &value) {
        quint8 result = static_cast<quint8>(data.at(2));
        value.insert(ACK_RESULT, result == 0 ? "成功" : "失败");
        if (result != 0) {
            quint8 error = static_cast<quint8>(data.at(3));
            switch (error) {
                case 0:
                    value.insert(ACK_ERROR, "无错误");
                    break;
                case 1:
                    value.insert(ACK_ERROR, "报警阈值超范围");
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
        case CMD_OPEN_CLOSE_PRINT_42:
            readPrintDataAck(data, value);
            break;
        case CMD_SET_ALARM_THRESHOLD_44:
            readAlarmThresholdAck(data, value);
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

bool Handler32data::readTemperatureHumidity(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 两字节的温度，两字节的湿度
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取温度
    qint16 temperature = 0;
    memcpy(&temperature, data.data(), 2);
    value.insert(ACK_TEMPERATURE, temperature / 10.0);

    // 读取湿度
    qint16 humidity = 0;
    memcpy(&humidity, data.data() + 2, 2);
    value.insert(ACK_HUMIDITY, humidity / 10.0);

    return true;
}

bool Handler32data::readGasProbeADC(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 共4字节，前两个字节代表ADC值
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取ADC值
    quint16 adc = 0;
    memcpy(&adc, data.data(), 2);
    value.insert(ACK_ADC_VALUE, adc);

    return true;
}

bool Handler32data::readGasProbeResistance(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 单位为K欧姆，数据类型为float，将4字节组合在一起
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取电阻值
    float resistance = 0;
    memcpy(&resistance, data.data(), 4);

    if (cmd == CMD_READ_RESISTANCE_23) {
        value.insert(ACK_RESISTANCE_VALUE, resistance);
    } else if (cmd == CMD_READ_COMPENSATION_RESISTANCE_24) {
        value.insert(ACK_COMPENSATION_RESISTANCE_VALUE, resistance);
    }

    return true;
}

bool Handler32data::readGasConcentration(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 4个字节，前两个为气体浓度，第三个为报警状态
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取浓度值
    quint16 concentration = 0;
    memcpy(&concentration, data.data(), 2);
    value.insert(ACK_GAS_CONCENTRATION, concentration);

    // 读取报警状态 TODO 缺少状态解释
    quint8 alarm = static_cast<quint8>(data.at(2));
    //    switch (alarm) {
    //        case 0:
    //            value.insert(ACK_ALARM_STATUS, "正常");
    //            break;
    //        case 1:
    //            value.insert(ACK_ALARM_STATUS, "报警");
    //            break;
    //        default:
    //            value.insert(ACK_ALARM_STATUS, "未知");
    //            break;
    //    }

    value.insert(ACK_ALARM_STATUS, alarm);

    return true;
}

bool Handler32data::readFirmwareVersion(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 4个字节，第一个字节为主版本号，第二个字节为副版本号
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    // 读取主版本号和副版本号，组成版本号
    quint8 mainVersion = static_cast<quint8>(data.at(0));
    quint8 subVersion = static_cast<quint8>(data.at(1));
    QString version = QString("%1.%2").arg(mainVersion).arg(subVersion);
    value.insert(ACK_FIRMWARE_VERSION, version);

    return true;
}

bool Handler32data::readProductFaultStatus(quint8 cmd, const QByteArray &data, QVariantMap &value)
{
    // 4个字节，第一个字节故障码
    if (data.length() != 4) {
        qWarning() << "cmd:" << cmd << "data length is not 4:" << data.toHex();
        return false;
    }

    quint8 fault = static_cast<quint8>(data.at(0));
    if (fault & (1 << 0))
        value.insert(ACK_ERROR, "内部错误 - 测量结果错误，比如：内部芯片通信故障");
    if (fault & (1 << 1))
        value.insert(ACK_ERROR, "数据超范围 - 传感器检测到的温湿度，气体浓度超范围了");
    if (fault & (1 << 3))
        value.insert(ACK_ERROR, "自检错误 - 设置错误等");
    if (fault & (1 << 4))
        value.insert(ACK_ERROR, "永久性损坏 - 任何无法恢复的传感器错误，需要更换传感器");
    if (fault & (1 << 5))
        value.insert(ACK_ERROR, "总运行时间超范围 - 生命达到了极限");

    return true;
}
