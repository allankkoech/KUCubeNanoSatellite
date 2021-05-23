#include "transreceiver.h"

TransReceiver::TransReceiver(QString ttyPortName, QString usbPort, QObject *parent)
    : QObject(parent), m_ttyPortName(ttyPortName), m_usbPort(usbPort)
{
    logs::debug("Starting to set up the TransReceiver port for LoRa.");

    m_serialPort = new QSerialPort(parent);

    connect(m_serialPort,&QSerialPort::readyRead,this,&TransReceiver::onReadyRead);

    connect(m_serialPort,&QSerialPort::errorOccurred,this,&TransReceiver::onSerialError);
}

TransReceiver::~TransReceiver()
{
    if(m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;

    logs::debug("Stopping the transreceiver and doing cleanup.");
}

void TransReceiver::init()
{
    m_portName="";

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        logs::debug(QString("Identifier: ")+(serialPortInfo.hasVendorIdentifier()? "true":"false")+"Product Identifier: "+serialPortInfo.hasProductIdentifier()+"Port Name: "+serialPortInfo.portName());

        if(serialPortInfo.portName().contains(m_ttyPortName, Qt::CaseSensitive))
        {
            m_portName = serialPortInfo.portName();

            logs::debug("Transreceiver Serial Port at '"+ m_portName+"'");

            break;
        }
    }

    if(m_portName.length()>0)
    {
        logs::debug("Starting configuring Transreceiver Serial Port with portName="+m_portName);

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
                    logs::warn("Error Opening Transreceiver Serial Port: "+QString::fromStdString(err.what()));
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
                logs::debug("Port opened, changing port configurations to: \n\tBAUD - 9600\n\tDATA - 8\n\tPARITY - NOPATITY CHECK\n\tSTOP BIT - ONESTOP\n\tFLOW CONTOL - NO FLOW CONTROL");

                m_serialPort->setBaudRate(QSerialPort::Baud9600);
                m_serialPort->setDataBits(QSerialPort::Data8);
                m_serialPort->setParity(QSerialPort::NoParity);
                m_serialPort->setStopBits(QSerialPort::OneStop);
                m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

                logs::debug("Configuring Transreceiver Port Completed Successfully.");


                return;
            }

            logs::debug("Transreceiver failed to be opened.");
        }

        catch (const std::exception &err)
        {
            logs::warn("Fatal Error while initializing Transreceiver :: " + QString::fromStdString(err.what()));
        }

        return;
    }

    return;
}

bool TransReceiver::onWriteData(const QString &data)
{
    if(m_serialPort->isOpen())
    {
        std::string str = data.toStdString();
        const char* serialData = str.c_str();

        logs::info(QString("Sending: ") + serialData);

        m_serialPort->write(serialData);

        m_serialPort->flush();

        logs::info("Data sent successfully!");

        return true;
    }
    else
    {
        logs::warn("Error sending data to Transmitter, Serial is Closed.");
        return false;
    }
}

void TransReceiver::onReadyRead()
{
    while(m_serialPort->canReadLine())
    {
        QString d=QString::fromUtf8(m_serialPort->readLine());

        logs::info("Transreceiver : "+d.remove("\r\n"));

        if(d.length()>0)
        {
            QStringList segmentedData=d.split(",");

            if(segmentedData.length()!=0)
            {
                if(segmentedData.length()==1)
                {
                    // Debug info received
                    logs::debug("Transreceiver log - "+segmentedData.at(0));
                }

                if(segmentedData.length()==15)
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

                            //emit gpsReceived(true, latitude, longitude);
                            logs::info("Valid Transreceiver data received LAT: "+QString::number(latitude)+" LONG: "+QString::number(longitude));
                            continue;
                        }
                    }
                }
            }
        }
    }
}

void TransReceiver::onSerialError(QSerialPort::SerialPortError error)
{
    if(error!=QSerialPort::NoError)
    {
        m_serialPort->close();

        logs::warn("Transreceiver Serial Port Error: "+QString(error));

        // emit error signal
    }
}
