/*
*    RayVessel
*    Copyright (C) 2024  RF Stateside LLC
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as
*    published by the Free Software Foundation, either version 3 of the
*    License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "helper.h"
#include <QCoreApplication>
#include <QSerialPortInfo>
#include <QDateTime>

helper::helper() {}

QList<QString> helper::GetCommPorts()
{
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    QList<QString> ports;

    for (const QSerialPortInfo& portInfo : serialPortInfos)
    {
#ifdef Q_OS_WIN
        if (portInfo.portName().startsWith("COM")) {
            ports.append(portInfo.portName());
        }
#else
        ports.append(portInfo.portName());
#endif
    }
    ports.sort();
    ports.insert(0, "None");

    return ports;

}

int helper::CalculateCheckSum(const QByteArray &data) {
    quint8 sum = 0;
    for (quint8 b : data) {
        sum += b;
    }
    return 256 - sum;
}

bool helper::VerifyCheckSum(const QByteArray &data) {
    if (data.size() < 2) {
        return false;  // Not enough data to contain a checksum
    }

    quint8 sum = 0;
    for (int i = 0; i < data.size() - 1; ++i) {
        sum += static_cast<quint8>(data[i]);
    }

    if (static_cast<quint8>(data[data.size() - 1]) == 256 - sum) {
        return true;
    } else {
        return false;
    }
}

void helper::Edit2ByteInt(QByteArray &data, int offset, int val)
{
    data[offset + 1] = static_cast<char>(val);
    data[offset] = static_cast<char>(val >> 8);
}

void helper::Edit2ByteDouble(QByteArray &data, int offset, double val, double scale)
{
    int ival = static_cast<int>(std::round(val / scale));
    data[offset + 1] = static_cast<char>(ival);
    data[offset] = static_cast<char>(ival >> 8);
}

bool helper::FileExists(const QString &filePath)
{
    QFileInfo checkFile(filePath);
    return checkFile.exists() && checkFile.isFile();
}

bool helper::IsDoubleValid(double value)
{
    return !qIsNaN(value) && !qIsInf(value) && value != 0.0;
}

bool helper::IsDaysSince1970Valid(int daysSince1970)
{
    // Get the current date
    QDate yesterdayDate = QDate::currentDate().addDays(-1);

    // Calculate the future date, 20 years from now
    QDate futureDate = yesterdayDate.addYears(20);

    // Calculate the date from DaysSince1970
    QDate dateFromDays = QDate::fromJulianDay(QDate(1970, 1, 1).toJulianDay() + daysSince1970);

    // Check if the date is valid and within the range
    return dateFromDays.isValid() && dateFromDays >= yesterdayDate && dateFromDays <= futureDate;
}

QString helper::GetTodaysDate()
{
    QDate currentDate = QDate::currentDate();
    return currentDate.toString("MM/dd/yyyy");
}

QList<QString> helper::GetListTripLogs(const QDir &folderPath)
{
    QList<QString> tripLogs;

    // Create a copy of folderPath to modify it
    QDir dirCopy = folderPath;

    // Set the name filters to look for .csv files
    QStringList filters;
    filters << "*trip*.csv";
    dirCopy.setNameFilters(filters);

    // Get the list of files in the directory
    QFileInfoList fileList = dirCopy.entryInfoList(QDir::Files);

    // Iterate through the files and add the ones that match the criteria to the list
    for (const QFileInfo &fileInfo : fileList) {
        if (fileInfo.fileName().contains("trip", Qt::CaseInsensitive) && fileInfo.suffix() == "csv")
        {
            tripLogs.append(fileInfo.fileName());
        }
    }

    return tripLogs;
}

QList<QString> helper::GetListFuelLogs(const QDir &folderPath)
{
    QList<QString> fuelLogs;

    // Create a copy of folderPath to modify it
    QDir dirCopy = folderPath;

    // Set the name filters to look for .csv files
    QStringList filters;
    filters << "*fuel*.csv";
    dirCopy.setNameFilters(filters);

    // Get the list of files in the directory
    QFileInfoList fileList = dirCopy.entryInfoList(QDir::Files);

    // Iterate through the files and add the ones that match the criteria to the list
    for (const QFileInfo &fileInfo : fileList) {
        if (fileInfo.fileName().contains("fuel", Qt::CaseInsensitive) && fileInfo.suffix() == "csv") {
            fuelLogs.append(fileInfo.fileName());
        }
    }

    return fuelLogs;
}


bool helper::createEmptyFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not create file:" << filePath;
        return false;
    }

    file.close();
    return true;
}

QString helper::GetApplicationPath()
{
    return QCoreApplication::applicationDirPath();
}

QString helper::GetSecretKey()
{
    QString m_secretKey = "";
    // Path to the secret key file
    QString appName = QString(APP_NAME).toLower();
    QString dirPath = QDir::homePath() + "/." + appName;
    QString keyFilePath = dirPath + "/.secret.key";
    QFile keyFile(keyFilePath);
    if (keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&keyFile);
        QString line;
        while (!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith("APP_SECRET_KEY=")) {
                m_secretKey = line.section('=', 1, 1);
                break;
            }
        }
        keyFile.close();
    } else {
        qCritical() << "Could not open the secret key file for reading:" << keyFile.errorString();
        return QString();
    }
    //qDebug() << "Secret key loaded from file:" << m_secretKey;
    return m_secretKey;
}

int helper::GetLocalOffsetFromSystemTime()
{
    // Get the current local time
    QDateTime localDateTime = QDateTime::currentDateTime();

    // Get the current UTC time
    QDateTime utcDateTime = localDateTime.toUTC();

    // Calculate the offset in seconds
    int offsetInSeconds = localDateTime.offsetFromUtc();

    return offsetInSeconds;

/*    // Optionally, convert the offset to hours and minutes
    int offsetHours = offsetInSeconds / 3600;
    return offsetHours;
*/
}

double helper::GetRpiCpuTemperature()
{
    // Path to the file that contains the CPU temperature
    QFile file("/sys/class/thermal/thermal_zone0/temp");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open the CPU temperature file";
        return -1;  // Return an error value
    }

    QTextStream in(&file);
    QString tempString = in.readLine();
    file.close();

    bool ok;
    double temperature = tempString.toDouble(&ok) / 1000.0; // Convert from millidegrees to degrees
    if (!ok) {
        qWarning() << "Failed to convert temperature string to double";
        return -1;  // Return an error value
    }

    return temperature;
}

/**
 * @brief Corrects the date of a QDateTime object to the current date while keeping the time.
 * @param sensorDateTime The QDateTime object to correct.
 * @return The corrected QDateTime object.
 */
QDateTime helper::correctDateTime(const QDateTime &sensorDateTime)
{
    // Get the current system date
    QDate currentDate = QDate::currentDate();

    // Extract the time from the sensor's QDateTime
    QTime sensorTime = sensorDateTime.time();

    // Check if the date from the sensor matches the current date
    if (sensorDateTime.date() != currentDate) {
        // If the date doesn't match, correct it to the current date while keeping the time
        QDateTime correctedDateTime(currentDate, sensorTime);
        return correctedDateTime;
    }

    // If the date matches, return the original QDateTime
    return sensorDateTime;
}
