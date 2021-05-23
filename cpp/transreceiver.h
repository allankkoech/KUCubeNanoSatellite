#ifndef TRANSRECEIVER_H
#define TRANSRECEIVER_H

#include <QObject>

class TransReceiver : public QObject
{
    Q_OBJECT
public:
    explicit TransReceiver(QObject *parent = nullptr);

signals:

};

#endif // TRANSRECEIVER_H
