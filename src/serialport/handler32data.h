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
    static bool frameIsValid(const QByteArray &frameData);

    bool readOperateResult(quint8 cmd, const QByteArray &data, QVariantMap &value);
    bool readOperateData(quint8 cmd, const QByteArray &data, QVariantMap &value);

private:
    QByteArray m_receivedData;

    typedef bool (Handler32data::*readFunc)(quint8 cmd, const QByteArray &data, QVariantMap &value);
    QMap<quint8 , readFunc> m_readFuncMap;
};


#endif //R32_HANDLER32DATA_H
