#ifndef RTCCONTROLLER_H
#define RTCCONTROLLER_H

#include <QObject>

class RTCController : public QObject
{
    Q_OBJECT
public:
    explicit RTCController(QObject *parent = nullptr);

signals:

};

#endif // RTCCONTROLLER_H
