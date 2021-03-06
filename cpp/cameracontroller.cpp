#include "cameracontroller.h"

CameraController::CameraController(QObject *parent) : QObject(parent)
{
    logs::debug("Starting Camera Controller Setup.");
    m_VideoCapture = new cv::VideoCapture();
}

CameraController::~CameraController()
{
    delete m_cameraRunnable;

    delete m_VideoCapture;

    logs::debug("Closing Camera Controller and cleaning up.");
}

void CameraController::init()
{
    logs::debug("Initializing VideoCapture in Camera Controller.");

    try
    {
        m_VideoCapture->open(0, cv::CAP_ANY);

        m_available=m_VideoCapture->isOpened();

        if(m_available)
        {
            m_cameraRunnable = new CameraRunnable(this,m_VideoCapture);

            connect(m_cameraRunnable,SIGNAL(finished(bool,cv::Mat)),this,SLOT(processFrame(bool,cv::Mat)));

            runCameraRunnable();

            logs::debug("All setup in Camera Confuguration");
        }
    }

    catch(cv::Exception& e)
    {
        logs::warn("Failed to initialize Camera: "+QString::fromStdString(e.what()));
    }

    catch(std::exception& e)
    {
        logs::warn("Failed to initialize Camera: "+QString::fromStdString(e.what()));
    }

    catch(...)
    {
        logs::warn("Failed to initialize Camera: Unknown Exception");
    }
}

void CameraController::processFrame(bool read, cv::Mat frame)
{
    if(read)
    {
        frame.copyTo(m_Frame);

        m_init=true;
    }

    QEventLoop loop;

    QTimer timer;

    timer.connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(1);

    loop.exec();

    runCameraRunnable();
}

void CameraController::runCameraRunnable()
{
    m_ThreadPool.start(m_cameraRunnable);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// RUNNABLE CLASS ///////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////////////////

CameraRunnable::CameraRunnable(QObject *parent, cv::VideoCapture *videoCapture):QObject(parent),m_VideoCapture(videoCapture)
{
    setAutoDelete(false);
}

void CameraRunnable::run()
{
    cv::Mat frame;

    if(m_VideoCapture->read(frame))
    {
        emit finished(true,frame.clone());
    }
    else
    {
        emit finished(false,frame.clone());
    }
}
