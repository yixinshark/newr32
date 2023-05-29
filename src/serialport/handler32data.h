//
// Created by yixin on 2023/5/28.
//

#ifndef R32_HANDLER32DATA_H
#define R32_HANDLER32DATA_H

#include <QObject>

#include <QMap>

class Handler32data : public QObject
{
    Q_OBJECT
public:
    explicit Handler32data(QObject *parent = nullptr);
    ~Handler32data() override;

signals:
    void frameReceived(quint8 cmd, const QVariantMap &info);
    void recvedFrameData(const QByteArray &frameData);

public slots:
    void processReceivedData(const QByteArray &data);

private:
    // 帧数据是否有效
    static bool frameIsValid(const QByteArray &frameData);

    // 读取操作结果
    bool readOperateResult(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取操作后的数据
    bool readOperateData(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取温湿度
    bool readTemperatureHumidity(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取气体探头的ADC采样值
    bool readGasProbeADC(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取气体探头的阻值 / 4.读取温度补偿后的气体探头阻值
    bool readGasProbeResistance(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取气体浓度ppm和报警状态值
    bool readGasConcentration(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 读取固件版本号
    bool readFirmwareVersion(quint8 cmd, const QByteArray &data, QVariantMap &value);
    // 查询产品的故障状态
    bool readProductFaultStatus(quint8 cmd, const QByteArray &data, QVariantMap &value);

private:
    QByteArray m_receivedData;

    typedef bool (Handler32data::*readFunc)(quint8 cmd, const QByteArray &data, QVariantMap &value);
    QMap<quint8 , readFunc> m_readFuncMap;
};


#endif //R32_HANDLER32DATA_H
