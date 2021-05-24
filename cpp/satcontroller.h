#ifndef SATCONTROLLER_H
#define SATCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <opencv2/core/mat.hpp>

#include "gpscontroller.h"
#include "cameracontroller.h"
#include "transreceiver.h"
#include "cputemperature.h"
#include "logger.h"
#include "globals.h"

class SatController : public QObject
{
    Q_OBJECT
public:
    explicit SatController(QObject *parent = nullptr);

    ~SatController();

signals:
    void transmitData(QString data);

private slots:
    void onGPSDataReceived(bool valid, double lat, double lon);

    void sendDataOverLora(const QString &data);

    void receivedDataFromLora(const QString &data);

    void onSyncTimerTimeout();

    void onCpuTemptReceived(const float &t);

private:
    GPSController * gpsController;

    CameraController *cameraController;

    TransReceiver * transReceiver;

    CpuTemperatureMonitor * cpuTempMonitor;

    QTimer * synctimer;

    bool m_isValidGps=false;

    double m_latitude=0, m_longitude=0, m_cpu_tempt=0;

    QString m_cwdPath;
};

#endif // SATCONTROLLER_H
