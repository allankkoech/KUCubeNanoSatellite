#include "satcontroller.h"

SatController::SatController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    // Initialize GPS Controller
    gpsController = new GPSController("ttyACM");

    gpsController->init();

    // Initialize Camera Controller
    cameraController = new CameraController();

    cameraController->init();

    // Initialize transreceiver module
    transReceiver = new TransReceiver("ttyUSB", "");

    transReceiver->init();

    connect(gpsController, &GPSController::gpsReceived, this, &SatController::onGPSDataReceived);

    connect(this, &SatController::transmitData, transReceiver, &TransReceiver::onWriteData);
}

SatController::~SatController()
{
    delete gpsController;

    delete cameraController;

    delete transReceiver;
}

void SatController::onGPSDataReceived(bool valid, double lat, double lon)
{
    m_isValidGps = valid;
    m_latitude = lat;
    m_longitude = lon;
}

void SatController::sendDataOverLora(const QString &data)
{
    emit transmitData(data);
}

void SatController::receivedDataFromLora(const QString &data)
{
    Q_UNUSED(data)
}
