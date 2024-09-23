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

#ifndef HELPER_H
#define HELPER_H

#include <qstring.h>
#include <QByteArray>
#include <QDebug>
#include <cmath> // For std::round
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QFileInfo>
#include <QFile>
#include <QDir>


class helper
{
public:
    helper();
    static QList<QString> GetCommPorts();
    static int CalculateCheckSum(const QByteArray &data);
    static bool VerifyCheckSum(const QByteArray &data);
    static void Edit2ByteInt(QByteArray &data, int offset, int val);
    static void Edit2ByteDouble(QByteArray &data, int offset, double val, double scale = 1.0);

    static bool FileExists(const QString &filePath);
    static bool IsDoubleValid(double value);
    static bool IsDaysSince1970Valid(int daysSince1970);
    static QString GetTodaysDate();
    static QList<QString> GetListTripLogs(const QDir &folderPath);
    static QList<QString> GetListFuelLogs(const QDir &folderPath);
    static bool createEmptyFile(const QString &filePath);
    static QString GetApplicationPath();
    static QString GetSecretKey();
    static int GetLocalOffsetFromSystemTime();
    static double GetRpiCpuTemperature();
    static QDateTime correctDateTime(const QDateTime& sensorDateTime);
};

#endif // HELPER_H
