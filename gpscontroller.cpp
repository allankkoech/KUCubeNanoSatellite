#include "gpscontroller.h"

GPSController::GPSController(QString ttyPortName, QObject *parent) : QObject(parent), m_ttyPortName(ttyPortName)
{
    m_serialPort = new QSerialPort(parent);

    connect(m_serialPort,&QSerialPort::readyRead,this,&GPSController::onReadyRead);

    connect(m_serialPort,&QSerialPort::errorOccurred,this,&GPSController::gpsSerialPortError);
}

GPSController::~GPSController()
{
    if(m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;
}

bool GPSController::init()
{
    m_portName="";

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        qDebug()<<"Identifier: "<<serialPortInfo.hasVendorIdentifier()<<"Product Identifier: "<<serialPortInfo.hasProductIdentifier()<<"Port Name: "<<serialPortInfo.portName();

        if(serialPortInfo.portName().contains(m_ttyPortName, Qt::CaseSensitive))
        {
            m_portName = serialPortInfo.portName();

            qDebug()<<"GPS Serial Port at "<< m_portName;

            break;
        }
    }

    if(m_portName.length()>0)
    {
        m_serialPort->setPortName(m_portName);

        if(!m_serialPort->isOpen())
        {
            try
            {
                m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
            }

            catch (const std::exception &err)
            {
                qDebug()<<"Error Opening GPS Serial Port: "<<err.what();
            }

            if(!m_serialPort->isOpen())
            {
                m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
            }
        }

        else
        {
            m_serialPort->close();
            m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
        }

        if(m_isAvailable)
        {
            m_serialPort->setBaudRate(QSerialPort::Baud115200);
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::NoParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

            qDebug()<<"GPS Port Up and running!";

            return true;
        }

        qDebug()<<"No Serial GPS Device found";

        return false;
    }

    return false;
}

void GPSController::onReadyRead()
{
    QString data=QString::fromUtf8(m_serialPort->readAll());

    qInfo() << "Received Data: " << data;

    QStringList dataSplit = data.split("\n");

    foreach(QString d,dataSplit)
    {
        if(d.length()>0)
        {
            QStringList segmentedData=d.split(",");

            if(segmentedData.length()!=0)
            {
                if(segmentedData.length()==1)
                {
                    // Debug info received
                    qDebug() << "GPS::DEBUG: " << segmentedData.at(0);
                }

                if(segmentedData.length()==4)
                {
                    // Filter for GPGGA data lines only
                    if( segmentedData.at(0).contains("GPGGA", Qt::CaseSensitive) )
                    {
                        bool isDataValid = segmentedData.at(6).toInt();

                        if(isDataValid)
                        {
                            float longitude /*East*/, latitude /*North*/;
                            longitude = segmentedData.at(2).toDouble();
                            latitude = segmentedData.at(4).toDouble();

                            emit gpsReceived(true, latitude, longitude);
                            continue;
                        }
                    }
                }
            }
        }
    }
}

void GPSController::gpsSerialPortError(QSerialPort::SerialPortError error)
{
    if(error!=QSerialPort::NoError)
    {
        m_serialPort->close();

        qDebug()<<"GPS Serial Port Error: "<<error;

        // startedSerializerPort(m_serialPort->isOpen());
    }
}
