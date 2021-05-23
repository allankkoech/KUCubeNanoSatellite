#ifndef TRANSRECEIVER_H
#define TRANSRECEIVER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtDebug>
#include <QDateTime>

#include "logger.h"

class TransReceiver : public QObject
{
    Q_OBJECT
public:
    explicit TransReceiver(QString ttyPortName, QString usbPort, QObject *parent = nullptr);

    ~TransReceiver();

    void init();

signals:

public slots:
    bool onWriteData(const QString &data);

    void onReadyRead();

    void onSerialError(QSerialPort::SerialPortError error);

private:
    QSerialPort * m_serialPort;

    QString m_portName;

    QList<int> m_data;

    bool m_isAvailable;

    QString m_ttyPortName, m_usbPort;
};

#endif // TRANSRECEIVER_H
