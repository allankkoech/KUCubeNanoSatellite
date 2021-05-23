#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThreadPool>
#include <QTimer>
#include <QEventLoop>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

class CameraRunnable;

class CameraController : public QObject
{
    Q_OBJECT
public:
    explicit CameraController(QObject *parent = nullptr);

    ~CameraController();

    void init();

signals:

private slots:
    void processFrame(bool read,cv::Mat frame);

    void runCameraRunnable();

private:
    CameraRunnable * m_cameraRunnable;

    cv::VideoCapture * m_VideoCapture;

    cv::Mat m_Frame;

    QThreadPool m_ThreadPool;

    bool m_available, m_init;
};


class CameraRunnable :public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit CameraRunnable(QObject * parent = nullptr,cv::VideoCapture * videoCapture = nullptr);

    void run();

    cv::VideoCapture * m_VideoCapture;

signals:
    void finished(bool read,cv::Mat frame);
};

#endif // CAMERACONTROLLER_H
