#ifndef NMEA2000_ACTISENSE_H
#define NMEA2000_ACTISENSE_H

#include <QSerialPort>
#include "NMEA2000.h"
#include "N2kMsg.h"
#include "N2kMessages.h"

class NMEA2000_Actisense : public QObject, public tNMEA2000
{
    Q_OBJECT

public:
    explicit NMEA2000_Actisense(QSerialPort &serialPort, QObject *parent = nullptr);

    // CAN functions
    bool CANOpen();
    bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
    bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);
    bool SendMessage(const tN2kMsg &N2kMsg);

    // Product information methods
    unsigned short GetN2kVersion() const;
    unsigned short GetProductCode() const;
    const char* GetModelID() const;
    const char* GetSwCode() const;
    const char* GetModelVersion() const;
    const char* GetModelSerialCode() const;
    unsigned short GetCertificationLevel() const;
    unsigned short GetLoadEquivalency() const;

    // NMEA2000 message handling
    void HandleMsg(const tN2kMsg &N2kMsg);

    // New methods from NMEA2000.cpp
    void HandleCommandedAddress(uint64_t CommandedName, unsigned char NewAddress, int iDev);
    void HandleCommandedAddress(const tN2kMsg &N2kMsg);

    // ISO Request methods
    static void SetN2kPGN59904(tN2kMsg &N2kMsg, uint8_t Destination, unsigned long RequestedPGN);
    static bool ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN);

    // PGN List methods
    static void SetN2kPGN126464(tN2kMsg &N2kMsg, uint8_t Destination, tN2kPGNList tr, const unsigned long *PGNs);

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
    // Heartbeat method
    static void SetN2kPGN126993(tN2kMsg &N2kMsg, uint32_t timeInterval_ms, uint8_t sequenceCounter);
#endif

private slots:
    void onSerialDataAvailable();

signals:
    void nmea2000MessageReceived(tN2kMsg &N2kMsg);

private:
    QSerialPort &serialPort;
    QByteArray readBuffer;
    bool AddressChanged = false;
    bool DeviceInformationChanged = false;

    tProductInformation productInformation;  // Use this for product information

    uint8_t calculateChecksum(const QByteArray &data);
    void addChecksum(QByteArray &data);

    void AppendByteEscaped(unsigned char byteToAppend, QByteArray &message);
    QByteArray encodeMessage(const tN2kMsg &N2kMsg);
    tN2kMsg decodeMessage(const QByteArray &message);

    const char Escape = 0x10;
    const char StartOfText = 0x02;
    const char EndOfText = 0x03;
    const int NMEA2000DataLenOffset = 2;
    const int NMEA2000HeaderLen = 3;
    const char MsgTypeN2kRx = 0x93;
    const char MsgTypeN2kTx = 0x94;
    const char BTS = 0xD0;
    const char MaxDataLen = 223;
    const char DefaultSource = 0;
};

#endif // NMEA2000_ACTISENSE_H
