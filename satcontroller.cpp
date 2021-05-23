#include "satcontroller.h"

SatController::SatController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");


    qDebug() << "Starting GPS initialization ...";

    // Initialize GPS Controller
    gpsController = new GPSController("ttyACM");

    gpsController->init();

    qDebug() << "Starting Camera initialization ...";

    // Initialize Camera Controller
    cameraController = new CameraController();

    cameraController->init();

    connect(gpsController, &GPSController::gpsReceived, this, &SatController::onGPSDataReceived);
}

SatController::~SatController()
{
    delete gpsController;

    delete cameraController;
}

void SatController::onGPSDataReceived(bool valid, double lat, double lon)
{
    m_isValidGps = valid;
    m_latitude = lat;
    m_longitude = lon;
}
