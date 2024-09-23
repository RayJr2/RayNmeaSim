#include "nmea2000_actisense.h"
#include <QDebug>

NMEA2000_Actisense::NMEA2000_Actisense(QSerialPort &serialPort, QObject *parent)
    : QObject(parent)
    , tNMEA2000()
    , serialPort(serialPort)
{
    connect(&serialPort, &QSerialPort::readyRead, this, &NMEA2000_Actisense::onSerialDataAvailable);

    // Initialize product information, example values
    productInformation.N2kVersion = 1300;  // Example value
    productInformation.ProductCode = 12345;  // Example value
    strcpy(productInformation.N2kModelID, "NGT-1 Model");
    strcpy(productInformation.N2kSwCode, "1.0.0");
    strcpy(productInformation.N2kModelVersion, "1.0");
    strcpy(productInformation.N2kModelSerialCode, "123456789");
    productInformation.CertificationLevel = 1;  // Example value
    productInformation.LoadEquivalency = 1;  // Example value
}

// Implement the required CAN functions
bool NMEA2000_Actisense::CANOpen() {
    if (serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "NGT-1 serial port opened successfully";
        return true;
    } else {
        qDebug() << "Failed to open NGT-1 serial port";
        return false;
    }
}

bool NMEA2000_Actisense::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
    QByteArray frame;
    frame.append(reinterpret_cast<const char*>(&id), sizeof(id));  // Append ID
    frame.append(reinterpret_cast<const char*>(&len), sizeof(len));  // Append length
    frame.append(reinterpret_cast<const char*>(buf), len);  // Append data

    // Debug: Print the frame content to verify before writing
    qDebug() << "Frame to send (size " << frame.size() << "):" << frame.toHex();
    // Write frame to the serial port
    qint64 bytesWritten = serialPort.write(frame);
    if (bytesWritten != frame.size()) {
        qDebug() << "Failed to write the complete frame to the serial port.";
        return false;
    }

    // Flush the serial port to ensure data is sent
    serialPort.flush();

    qDebug() << "Successfully sent frame.";
    return true;
}

bool NMEA2000_Actisense::SendMessage(const tN2kMsg &N2kMsg)
{
    QByteArray message = encodeMessage(N2kMsg);
    qint64 bytesWritten = serialPort.write(message);
    if (bytesWritten != message.size()) {
        qDebug() << "Failed to write the complete message to the serial port.";
        return false;
    }
    serialPort.flush();
    return true;
}

bool NMEA2000_Actisense::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf) {
    QByteArray frame = serialPort.readAll();
    if (frame.size() >= 8) {  // Minimum size of a CAN frame
        id = *reinterpret_cast<const unsigned long*>(frame.data());
        len = frame[sizeof(id)];
        memcpy(buf, frame.data() + sizeof(id) + sizeof(len), len);
        return true;
    }
    return false;
}

// Implement required product information methods using productInformation
unsigned short NMEA2000_Actisense::GetN2kVersion() const {
    return productInformation.N2kVersion;
}

unsigned short NMEA2000_Actisense::GetProductCode() const {
    return productInformation.ProductCode;
}

const char* NMEA2000_Actisense::GetModelID() const {
    return productInformation.N2kModelID;
}

const char* NMEA2000_Actisense::GetSwCode() const {
    return productInformation.N2kSwCode;
}

const char* NMEA2000_Actisense::GetModelVersion() const {
    return productInformation.N2kModelVersion;
}

const char* NMEA2000_Actisense::GetModelSerialCode() const {
    return productInformation.N2kModelSerialCode;
}

unsigned short NMEA2000_Actisense::GetCertificationLevel() const {
    return productInformation.CertificationLevel;
}

unsigned short NMEA2000_Actisense::GetLoadEquivalency() const {
    return productInformation.LoadEquivalency;
}

// Implement message handling
void NMEA2000_Actisense::HandleMsg(const tN2kMsg &N2kMsg) {
    qDebug() << "Received N2kMsg with PGN:" << N2kMsg.PGN;
}

// Implement Commanded Address methods
void NMEA2000_Actisense::HandleCommandedAddress(uint64_t CommandedName, unsigned char NewAddress, int iDev) {
    if (Devices[iDev].DeviceInformation.GetName() == CommandedName && Devices[iDev].N2kSource != NewAddress) {
        Devices[iDev].N2kSource = NewAddress;
        Devices[iDev].UpdateAddressClaimEndSource();
        StartAddressClaim(iDev);
        AddressChanged = true;
    }
}

void NMEA2000_Actisense::HandleCommandedAddress(const tN2kMsg &N2kMsg) {
    if (N2kMsg.PGN != 65240L || !N2kMsg.IsTPMessage() || N2kMsg.DataLen != 9) return;

    int iDev = FindSourceDeviceIndex(N2kMsg.Destination);
    if (!tNMEA2000::IsBroadcast(N2kMsg.Destination) && iDev == -1) return;

    int Index = 0;
    uint64_t CommandedName = N2kMsg.GetUInt64(Index);
    unsigned char NewAddress = N2kMsg.GetByte(Index);
    if (NewAddress >= 252) return;

    HandleCommandedAddress(CommandedName, NewAddress, iDev);
}

// Implement ISO Request methods
void NMEA2000_Actisense::SetN2kPGN59904(tN2kMsg &N2kMsg, uint8_t Destination, unsigned long RequestedPGN) {
    N2kMsg.SetPGN(59904L);
    N2kMsg.Destination = Destination;
    N2kMsg.Priority = 6;
    N2kMsg.Add3ByteInt(RequestedPGN);
}

bool NMEA2000_Actisense::ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN) {
    if (N2kMsg.DataLen >= 3 && N2kMsg.DataLen <= 8) {
        int Index = 0;
        RequestedPGN = N2kMsg.Get3ByteUInt(Index);
        return true;
    }
    return false;
}

// Implement PGN List methods
void NMEA2000_Actisense::SetN2kPGN126464(tN2kMsg &N2kMsg, uint8_t Destination, tN2kPGNList tr, const unsigned long *PGNs) {
    N2kMsg.SetPGN(126464L);
    N2kMsg.Destination = Destination;
    N2kMsg.Priority = 6;
    N2kMsg.AddByte(tr);

    for (int i = 0; (pgm_read_dword(&PGNs[i])) != 0; i++) {
        N2kMsg.Add3ByteInt(pgm_read_dword(&PGNs[i]));
    }
}

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
static const uint32_t MaxHeartbeatInterval = 60000;  // Define max heartbeat interval as 60 seconds
void NMEA2000_Actisense::SetN2kPGN126993(tN2kMsg &N2kMsg, uint32_t timeInterval_ms, uint8_t sequenceCounter) {
    N2kMsg.SetPGN(126993L);
    N2kMsg.Priority = 7;
    if (timeInterval_ms > MaxHeartbeatInterval) {
        N2kMsg.Add2ByteUInt(0xfffe);  // Error
    } else {
        N2kMsg.Add2ByteUInt((uint16_t)(timeInterval_ms));
    }
    N2kMsg.AddByte(sequenceCounter);
    N2kMsg.AddByte(0xff);  // Reserved
    N2kMsg.Add4ByteUInt(0xffffffff);  // Reserved
}

void NMEA2000_Actisense::onSerialDataAvailable()
{
    // Append new data from the serial port to the readBuffer
    readBuffer.append(serialPort.readAll());

    // Process the buffer until we find valid NMEA2000 packets
    while (readBuffer.size() >= (NMEA2000HeaderLen + 2)) { // Minimum size of a packet
        // Find the start of the packet (Escape, StartOfText)
        int startIdx = readBuffer.indexOf(QByteArray::fromRawData("\x10\x02", 2)); // Escape (0x10), StartOfText (0x02)

        // If start of a packet is not found, clear the buffer and return
        if (startIdx == -1) {
            readBuffer.clear();
            return;
        }

        // Check if we have a valid message type (MsgTypeN2kData or MsgTypeN2kRequest)
        if (readBuffer.size() < startIdx + NMEA2000HeaderLen) {
            // Not enough data for the header, wait for more data
            return;
        }

        char messageType = readBuffer[startIdx + NMEA2000HeaderLen - 1];
        if (messageType != MsgTypeN2kRx && messageType != MsgTypeN2kTx) {
            // Invalid message type, remove everything before this position
            readBuffer.remove(0, startIdx + 1);
            continue;
        }

        // Find the end of the packet (Escape, EndOfText)
        int endIdx = readBuffer.indexOf(QByteArray::fromRawData("\x10\x03", 2), startIdx + NMEA2000HeaderLen);
        if (endIdx == -1) {
            // No complete packet found yet, wait for more data
            return;
        }

        // Extract the full packet
        QByteArray packet = readBuffer.mid(startIdx, endIdx - startIdx + 2); // Include the EndOfText (0x10 0x03)
        //qDebug() << "Rcvd packet:" << packet.toHex(' ').toUpper();

        // Remove the processed data from the buffer
        readBuffer.remove(0, endIdx + 2);

        // Now extract the message data (excluding the header and footer)
        QByteArray messageData = packet.mid(2, packet.size() - 4); // Remove 2 bytes from start and 2 bytes from end
        //qDebug() << "Rcvd af remove:" << messageData.toHex(' ').toUpper();

        // Get the checksum (last byte of messageData)
        quint8 receivedChecksum = static_cast<quint8>(messageData[messageData.size() - 1]);

        // Remove the checksum from the messageData
        messageData.chop(1);

        // Calculate the expected checksum
        uint8_t calculatedChecksum = calculateChecksum(messageData);

        // Validate checksum
        if (calculatedChecksum != receivedChecksum) {
            qWarning() << "Checksum mismatch: received" << receivedChecksum << "expected" << calculatedChecksum;
            continue; // Ignore the packet if checksum is incorrect
        }

        //qDebug() << "Rcvd valid packet:" << messageData.toHex(' ').toUpper();
        // Pass the valid messageData for further processing
        decodeMessage(messageData);
    }
}

uint8_t NMEA2000_Actisense::calculateChecksum(const QByteArray &data)
{
    int byteSum = 0;

    // Sum all the bytes in the data
    for (char byte : data) {
        byteSum += static_cast<uint8_t>(byte);
    }

    // The checksum is calculated as 256 - (byteSum % 256), and then reduced modulo 256
    return static_cast<uint8_t>(256 - (byteSum % 256));
}

void NMEA2000_Actisense::addChecksum(QByteArray &data)
{
    // Calculate checksum for the data
    uint8_t checksum = calculateChecksum(data);

    // Append the checksum to the data
    data.append(static_cast<char>(checksum));

    // Escape the checksum if necessary (e.g., if it matches the Escape character)
    if (checksum == Escape) {
        data.append(static_cast<char>(checksum)); // Double escape if necessary
    }
}

void NMEA2000_Actisense::AppendByteEscaped(unsigned char byteToAppend, QByteArray &message)
{
    message.append(static_cast<char>(byteToAppend));

    if (byteToAppend == 0x10) { // Escape character
        message.append(static_cast<char>(0x10)); // Double the escape if necessary
    }
}

QByteArray NMEA2000_Actisense::encodeMessage(const tN2kMsg &N2kMsg)
{
    QByteArray message;

    // Add Priority
    message.append(static_cast<char>(N2kMsg.Priority));

    // Add PGN (3 bytes)
    message.append(static_cast<char>(N2kMsg.PGN & 0xFF));         // First byte of PGN
    message.append(static_cast<char>((N2kMsg.PGN >> 8) & 0xFF));  // Second byte of PGN
    message.append(static_cast<char>((N2kMsg.PGN >> 16) & 0xFF)); // Third byte of PGN

    // Add Destination
    message.append(static_cast<char>(N2kMsg.Destination));

/*  // Commented out Source and Timestamp, Actisense NGT-1 does not use them
    // Add Source
    //message.append(static_cast<char>(N2kMsg.Source));
    message.append(static_cast<char>(0x00));

    // Add Timestamp (for now we can use 0 for simplicity)
    quint32 msgTime = 0; // Use appropriate value or keep as 0
    message.append(static_cast<char>(msgTime & 0xFF));
    message.append(static_cast<char>((msgTime >> 8) & 0xFF));
    message.append(static_cast<char>((msgTime >> 16) & 0xFF));
    message.append(static_cast<char>((msgTime >> 24) & 0xFF));
*/

    // Add Data Length
    message.append(static_cast<char>(N2kMsg.DataLen));

    // Add Payload (NMEA2000 PGN data)
    for (int i = 0; i < N2kMsg.DataLen; i++) {
        message.append(static_cast<char>(N2kMsg.Data[i]));
    }

    // Get size of the message
    uint8_t length = static_cast<uint8_t>(message.size());
    message.prepend(static_cast<char>(length));

    // Add message type
    message.prepend(MsgTypeN2kTx); // Message type (0x93)

    // Add checksum
    addChecksum(message);

    // Add PreAmble and PostAmble
    message.prepend(StartOfText); // Start of text (0x02)
    message.prepend(Escape);      // Escape character (0x10)
    message.append(Escape);       // Escape character (0x10)
    message.append(EndOfText);    // End of text (0x03)

    return message;
}

tN2kMsg NMEA2000_Actisense::decodeMessage(const QByteArray &message)
{
    //qDebug() << "Decoding message:" << message.toHex(' ').toUpper();

    tN2kMsg N2kMsg;
    int idx = 2; // Index to track the position in the byte array

    // Extract Priority
    N2kMsg.Priority = static_cast<uint8_t>(message[idx++]);
    //qDebug() << "Priority:" << N2kMsg.Priority;

    // Extract PGN (3 bytes)
    N2kMsg.PGN = static_cast<uint8_t>(message[idx++]);
    N2kMsg.PGN |= static_cast<uint8_t>(message[idx++]) << 8;
    N2kMsg.PGN |= static_cast<uint8_t>(message[idx++]) << 16;
    //qDebug() << "PGN:" << N2kMsg.PGN;

    // Extract Destination
    N2kMsg.Destination = static_cast<uint8_t>(message[idx++]);
    //qDebug() << "Destination:" << N2kMsg.Destination;

    // Extract Source
    N2kMsg.Source = static_cast<uint8_t>(message[idx++]);
    //qDebug() << "Source:" << N2kMsg.Source;

    // Skip Timestamp (4 bytes)
    idx += 4;

    // Extract Data Length
    N2kMsg.DataLen = static_cast<uint8_t>(message[idx++]);
    //qDebug() << "Data Length:" << N2kMsg.DataLen;

    // Extract Payload (NMEA2000 PGN data)
    for (int i = 0; i < N2kMsg.DataLen; i++) {
        N2kMsg.Data[i] = static_cast<uint8_t>(message[idx++]);
    }
    //qDebug() << "Data:" << QByteArray(reinterpret_cast<const char *>(N2kMsg.Data), N2kMsg.DataLen).toHex(' ').toUpper();

    //qDebug() << "Decoded N2kMsg with PGN:" << N2kMsg.PGN << "Data:" << N2kMsg.DataLen << "bytes";
    // Handle the decoded message
    emit nmea2000MessageReceived(N2kMsg); // Signal to your Nmea2000Handler class

    return N2kMsg;
}

#endif
