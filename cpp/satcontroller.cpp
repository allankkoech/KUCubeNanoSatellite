#include "satcontroller.h"
#include <QCoreApplication>

SatController::SatController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    m_cwdPath = qApp->applicationDirPath();
    logs::info("Working from :/CWD/: "+m_cwdPath);

    // Initialize GPS Controller
    gpsController = new GPSController(GPS_PORT_NAME);

    gpsController->init();

    // Initialize Camera Controller
    cameraController = new CameraController();

    cameraController->init();

    // Initialize transreceiver module
    transReceiver = new TransReceiver(TRANSRECEIVER_PORT_NAME, TRANSRECEIVER_USB_PORT);

    transReceiver->init();

    // Initialize Sync Timer
    synctimer = new QTimer(this);
    synctimer->setInterval(SYNC_INTERVAL_IN_MS);
    connect(synctimer, &QTimer::timeout, this, &SatController::onSyncTimerTimeout);
    synctimer->start();

    // Initialize CPU Temperature monitor
    cpuTempMonitor = new CpuTemperatureMonitor(m_cwdPath, this);

    connect(gpsController, &GPSController::gpsReceived, this, &SatController::onGPSDataReceived);

    connect(this, &SatController::transmitData, transReceiver, &TransReceiver::onWriteData);

    connect(cpuTempMonitor, &CpuTemperatureMonitor::temperatureChanged, this, &SatController::onCpuTemptReceived);
}

SatController::~SatController()
{
    delete gpsController;

    delete cameraController;

    delete transReceiver;

    delete cpuTempMonitor;
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

void SatController::onSyncTimerTimeout()
{
    // logs::debug("Sync Timeout");

    auto t = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();

    QString payload = "=>"+QString::number(t)+":"+QString::number(m_latitude);
    payload+=":"+QString::number(m_longitude)+"=>";

    logs::info("Payload to send: "+payload);
    sendDataOverLora(payload);

}

void SatController::onCpuTemptReceived(const float &t)
{
    m_cpu_tempt = t;
}
