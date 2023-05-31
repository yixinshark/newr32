//
// Created by zyz on 2023/5/31.
//

#include "handlemcudata.h"
#include "mcuconstant.h"

#include <QDebug>
#include <QVariantMap>

HandleMcuData::HandleMcuData(QObject *parent)
    : QObject(parent)
{
}

HandleMcuData::~HandleMcuData()
{

}

QByteArray HandleMcuData::getSendData(char cmd, const QVariantMap &info)
{
    QByteArray data;
    addContent(cmd, info, data);

    return data;
}

void HandleMcuData::processReceivedData(const QByteArray &data)
{

}

void HandleMcuData::addContent(char cmd, const QVariantMap &info, QByteArray &data)
{
    data.append(MCU_SEND_HEADER);
    data.append(cmd);

    switch (cmd) {
        case MCU_CMD_VALVE:
            addCmd_valve_Content(info, data);
            break;
        case MCU_CMD_FAN:
            addCmd_fan_Content(info, data);
            break;
        case MCU_CMD_ND:
            addCmd_nd_Content(info, data);
            break;
        case MCU_CMD_CHANNEL:
            addCmd_set_channel_Content(info, data);
            break;
        case MCU_CMD_BOX:
            addCmd_box_switch_Content(info, data);
            break;
        case MCU_CMD_ALARM_LIGHT:
            addCmd_alarm_light_Content(info, data);
            break;
    }

    addCheckSum(data);
}

void HandleMcuData::addCheckSum(QByteArray &data)
{
    quint16 checksum = 0;
    for (char i : data)
        checksum += static_cast<quint8>(i);

    data.append(static_cast<char>((checksum >> 8) & 0xFF));
    data.append(static_cast<char>(checksum & 0xFF));
}

bool HandleMcuData::addCmd_valve_Content(const QVariantMap &info, QByteArray &data)
{
    if (info.contains(MCU_VALVE)) {
        data.append(info[MCU_VALVE].toByteArray());
        return true;
    }

    qWarning() << "valve not exist:" << info;
    return false;
}

bool HandleMcuData::addCmd_fan_Content(const QVariantMap &info, QByteArray &data)
{
    if (info.contains(MCU_FAN)) {
        data.append(info[MCU_FAN].toByteArray());
        data.append((char)0x00); // 填充一个字节
        return true;
    }

    qWarning() << "fan not exist:" << info;
    return false;
}

bool HandleMcuData::addCmd_nd_Content(const QVariantMap &info, QByteArray &data)
{
    if (info.contains(MCU_ND)) {
        data.append(info[MCU_ND].toByteArray());
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        return true;
    }

    qWarning() << "mcu nd not exist:" << info;
    return false;
}

bool HandleMcuData::addCmd_set_channel_Content(const QVariantMap &info, QByteArray &data)
{
    if (info.contains(MCU_SET_CHANNEL)) {
        char channel = static_cast<char>(info[MCU_SET_CHANNEL].toUInt());
        data.append(channel);
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节

        return true;
    }

    qWarning() << "mcu set channel not exist:" << info;
    return false;
}

bool HandleMcuData::addCmd_box_switch_Content(const QVariantMap &info, QByteArray &data)
{
    if (info.contains(MCU_BOX_SWITCH)) {
        char box_switch = static_cast<char>(info[MCU_BOX_SWITCH].toUInt());
        data.append(box_switch);
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节
        data.append((char)0x00); // 填充一个字节

        return true;
    }

    return false;
}

bool HandleMcuData::addCmd_alarm_light_Content(const QVariantMap &info, QByteArray &data)
{
    if (!info.contains(MCU_BOX_SWITCH) || !info.contains(MCU_ALARM_LIGHT)) {
        qWarning() << "mcu alarm light not exist:" << info;
        return false;
    }

    char box_switch = static_cast<char>(info[MCU_BOX_SWITCH].toUInt());
    char alarm_light = static_cast<char>(info[MCU_ALARM_LIGHT].toUInt());
    data.append(box_switch);
    data.append(alarm_light);

    data.append((char)0x00); // 填充一个字节
    data.append((char)0x00); // 填充一个字节
    data.append((char)0x00); // 填充一个字节

    return true;
}


