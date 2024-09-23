#ifndef NMEA2000HANDLER_H
#define NMEA2000HANDLER_H

#include <QObject>
#include <QSerialPort>
#include "nmea2000_actisense.h"

class Nmea2000Handler : public QObject {
    Q_OBJECT

public:
    explicit Nmea2000Handler(QObject *parent = nullptr);
    void InitializeActisense(const QString &portName, QSerialPort::BaudRate baudRate = QSerialPort::Baud115200);
    void sendTestPgn129026();
    void SendPgn129026(double COG, double SOG, tN2kHeadingReference COGReference = N2khr_magnetic);

public slots:
    void nmea2000MessageReceived(tN2kMsg &N2kMsg);

private:
    QSerialPort serialPort;
    NMEA2000_Actisense *nmea2000Actisense;
};

#endif // NMEA2000HANDLER_H
