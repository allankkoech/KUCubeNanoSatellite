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
        logs::debug("Starting configuring GPS Serial Port with portName="+m_portName);

        m_serialPort->setPortName(m_portName);

        try {

            if(!m_serialPort->isOpen())
            {
                logs::debug("Attempting to open the serial port since its closed.");

                try
                {
                    m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
                }

                catch (const std::exception &err)
                {
                    logs::warn("Error Opening GPS Serial Port: "+QString::fromStdString(err.what()));
                }

                if(!m_serialPort->isOpen())
                {
                    m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
                }
            }

            else
            {
                logs::debug("Attempting restart Serial Port since its already Open.");

                m_serialPort->close();
                m_isAvailable=m_serialPort->open(QSerialPort::ReadWrite);
            }

            if(m_isAvailable)
            {
                logs::debug("Port opened, changing port configurations to \n\tBAUD - 115200\n\tDATA - 8\n\tPARITY - NOPATITY CHECK\n\tSTOP BIT - ONESTOP\n\tFLOW CONTOL - NO FLOW CONTROL");

                m_serialPort->setBaudRate(QSerialPort::Baud115200);
                m_serialPort->setDataBits(QSerialPort::Data8);
                m_serialPort->setParity(QSerialPort::NoParity);
                m_serialPort->setStopBits(QSerialPort::OneStop);
                m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

                logs::debug("Configuring GPS Port Completed.");


                return true;
            }

            logs::debug("GPS failed to be opened.");
        }

        catch (const std::exception &err)
        {
            logs::warn("Fatal Error while initializing GPS :: " + QString::fromStdString(err.what()));
        }

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
