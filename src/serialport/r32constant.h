//
// Created by yixin on 2023/5/28.
//

#ifndef R32_R32CONSTANT_H
#define R32_R32CONSTANT_H

#include <QString>

const quint8 SEND_FRAME_LENGTH = 8;
const quint8 RECV_FRAME_LENGTH = 7;

const quint8 SEND_HEADER = 0x65;
const quint8 RECV_HEADER = 0x66;

// 标定浓度
const quint8 CMD_ND_01 = 0x01;
// 标定浓度完成命令码
const quint8 CMD_ND_OVER_02 = 0x02;
// 查询标定状态命令码
const quint8 CMD_ND_STATUS_03 = 0x03;
// 读取标定的零点阻值R0
const quint8 CMD_READ_R0_04 = 0x04;
// 读取标定参数1
const quint8 CMD_READ_PARAM1_05 = 0x05;
// 读取标定参数2
const quint8 CMD_READ_PARAM2_06 = 0x06;
// 读取1000ppm的Rs/R0
const quint8 CMD_READ_1000PPM_07 = 0x07;
// 读取5000ppm的Rs/R0
const quint8 CMD_READ_5000PPM_08 = 0x08;



// 常量定义
const QString ACK_RESULT = "result";
const QString ACK_ERROR = "error";
// 标定的零点阻值R0
const QString ACK_R0_VALUE = "R0_value";
// 标定参数1
const QString ACK_PARAM1_VALUE = "param1_value";
// 标定参数2
const QString ACK_PARAM2_VALUE = "param2_value";
// 1000ppm的Rs/R0
const QString ACK_1000PPM_VALUE = "1000ppm_value";
// 5000ppm的Rs/R0
const QString ACK_5000PPM_VALUE = "5000ppm_value";



#endif //R32_R32CONSTANT_H
