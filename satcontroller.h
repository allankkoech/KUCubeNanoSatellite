#ifndef SATCONTROLLER_H
#define SATCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <opencv2/core/mat.hpp>

#include "gpscontroller.h"
#include "cameracontroller.h"
#include "logger.h"

class SatController : public QObject
{
    Q_OBJECT
public:
    explicit SatController(QObject *parent = nullptr);

    ~SatController();

signals:

private slots:
    void onGPSDataReceived(bool valid, double lat, double lon);

private:
    GPSController * gpsController;

    CameraController *cameraController;

    bool m_isValidGps=false;

    double m_latitude=0, m_longitude=0;
};

#endif // SATCONTROLLER_H
