#ifndef CPUTEMPERATURE_H
#define CPUTEMPERATURE_H

#include <QObject>
#include <QTimer>
#include <QProcess>

#include "logger.h"

class CpuTemperatureMonitor : public QObject
{
    Q_OBJECT
public:
    explicit CpuTemperatureMonitor(QString cwd, QObject *parent = nullptr);

    ~CpuTemperatureMonitor();

signals:
    void temperatureChanged(float t);

private slots:
    void onReadyRead();

    void onProcessError(QProcess::ProcessError error);

    void fetchCpuTemperature();

private:
    QString m_cwd;

    QTimer * m_timer;

    QProcess * m_temperatureProbeProcess;

};

inline CpuTemperatureMonitor::CpuTemperatureMonitor(QString cwd, QObject *parent): QObject(parent), m_cwd(cwd)
{
    logs::debug("Starting CPU logging monitor class");

    m_temperatureProbeProcess = new QProcess(this);

    connect(m_temperatureProbeProcess, &QProcess::errorOccurred, this, &CpuTemperatureMonitor::onProcessError);
    connect(m_temperatureProbeProcess, &QProcess::readyReadStandardOutput, this, &CpuTemperatureMonitor::onReadyRead);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &CpuTemperatureMonitor::fetchCpuTemperature);
    m_timer->start();

}

inline CpuTemperatureMonitor::~CpuTemperatureMonitor()
{
    if(m_temperatureProbeProcess->isOpen())
        m_temperatureProbeProcess->close();

    delete m_temperatureProbeProcess;

    logs::debug("Closing CPU Temperature monitoring");
}

inline void CpuTemperatureMonitor::onReadyRead()
{
    auto _d = m_temperatureProbeProcess->readAll();
    auto d = _d.data();
    logs::info("Data: "+QString(d));

    emit temperatureChanged(QString(d).toFloat());
}

inline void CpuTemperatureMonitor::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    logs::warn("Error fetching CPU Temperature: "+QString(m_temperatureProbeProcess->errorString()));
}

inline void CpuTemperatureMonitor::fetchCpuTemperature()
{
    QString cmd = m_cwd+"/bash/cpu_tempt.sh";
    m_temperatureProbeProcess->start(cmd, QStringList());
}

#endif // CPUTEMPERATURE_H
