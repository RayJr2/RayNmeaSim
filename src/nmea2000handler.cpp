#include "nmea2000handler.h"
#include <QDebug>
#include <QThread>
#include "N2kMessages.h"

extern NMEA2000_Actisense nmea2000Actisense;

Nmea2000Handler::Nmea2000Handler(QObject *parent)
    : QObject(parent)
    , nmea2000Actisense(nullptr)
{}

void Nmea2000Handler::InitializeActisense(const QString &portName, QSerialPort::BaudRate baudRate)
{
    // Set up the serial port for Actisense NGT-1
    serialPort.setPortName(portName);
    serialPort.setBaudRate(baudRate);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    QThread::msleep(10);
    // Create the NMEA2000_Actisense object
    nmea2000Actisense = new NMEA2000_Actisense(serialPort);
    QThread::msleep(10);
    nmea2000Actisense->SetMode(tNMEA2000::N2km_ListenAndNode, 44);
    QThread::msleep(10);

    // Open the NMEA2000 connection
    if (nmea2000Actisense->Open()) {
        qInfo() << "NGT-1 connected";
    } else {
        qWarning() << "Failed to connect to NGT-1";
    }
}

void Nmea2000Handler::sendTestPgn129026() {
    tN2kMsg N2kMsg;

    // Dummy data for PGN 129026 (COG & SOG, Rapid Update)
    double COG = 1.2345;      // Course over ground (in radians)
    double SOG = 2.3456;      // Speed over ground (in m/s)
    tN2kHeadingReference COGReference = N2khr_magnetic;

    // Create the PGN 129026 message
    //SetN2kCOGSOGRapid(N2kMsg, 1, COGReference, COG, SOG);
    SetN2kPGN129026(N2kMsg, 1, COGReference, COG, SOG);

    // Send the message via NMEA2000
    if (nmea2000Actisense->SendMessage(N2kMsg)) {
        //qDebug() << "PGN 129026 sent successfully";
    } else {
        qDebug() << "Failed to send PGN 129026";
    }
}

void Nmea2000Handler::SendPgn129026(double COG, double SOG, tN2kHeadingReference COGReference)
{
    tN2kMsg N2kMsg;

    // Create the PGN 129026 message
    SetN2kPGN129026(N2kMsg, 1, COGReference, COG, SOG);

    // Send the message via NMEA2000
    if (nmea2000Actisense->SendMessage(N2kMsg)) {
        //qDebug() << "PGN 129026 sent successfully";
    } else {
        qWarning() << "Failed to send PGN 129026";
    }
}

void Nmea2000Handler::nmea2000MessageReceived(tN2kMsg &N2kMsg) {}
