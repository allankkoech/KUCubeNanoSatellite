#ifndef GPSCONTROLLER_H
#define GPSCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtDebug>
#include <QDateTime>

class GPSController : public QObject
{
    Q_OBJECT
public:
    explicit GPSController(QString ttyPortName, QObject *parent = nullptr);

    ~GPSController();

    bool init();

signals:
    void gpsReceived(bool valid, double latitude, double longitude);


private slots:
    void onReadyRead();

    void gpsSerialPortError(QSerialPort::SerialPortError error);

private:
    QSerialPort * m_serialPort;

    QString m_portName;

    QList<int> m_data;

    bool m_isAvailable;

    QString m_ttyPortName;
};

#endif // GPSCONTROLLER_H
