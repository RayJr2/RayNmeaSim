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

#include "convert.h"
//#include "helper.h" // Removed 8/29/2024 v0.5.3

Convert::Convert()
{

}

double Convert::FahrenheitToKelvin(double tempF)
{
    return (tempF - 32) * 5.0 / 9.0 + 273.15;
}

double Convert::KelvinToFahrenheit(double tempK)
{
    return (tempK - 273.15) * 9.0 / 5.0 + 32;
}

double Convert::CelsiusToKelvin(double tempC)
{
    return tempC + 273.15;
}

double Convert::KelvinToCelsius(double tempK)
{
    return tempK - 273.15;
}

double Convert::CelsiusToFahrenheit(double tempC)
{
    return tempC * 9 / 5 + 32;
}

double Convert::FahrenheitToCelsius(double tempF)
{
    return (tempF - 32) * 5 / 9;
}

double Convert::PsiToPascal(double psi)
{
    return psi * 6894.75729;
}

double Convert::PascalToPsi(double pascal)
{
    return pascal / 6894.75729;
}

//Gallon per hour(US) to Liter per hour
double Convert::GphToLph(double gph)
{
    return gph * 3.785411784;
}

double Convert::LphToGph(double lph)
{
    return lph / 3.785411784;
}

//Gallon per hour(US) to Cubic Meters per hour
double Convert::GphToCmph(double gph)
{
    return gph * .003785411784;
}

double Convert::CmphToGph(double cmph)
{
    return cmph / 0.003785411784;
}

double Convert::HoursToSeconds(double hours)
{
    return hours * 3600;
}

double Convert::SecondsToHours(double seconds)
{
    return seconds / 3600;
}

double Convert::MinutesToHours(double minutes)
{
    return minutes / 60;
}

double Convert::HoursToMinutes(double hours)
{
    return hours * 60;
}

QString Convert::SecondsToFormattedTime_hhmmss(double seconds)
{
    // Calculate hours, minutes, and seconds
    int totalSeconds = static_cast<int>(seconds);
    int hrs = totalSeconds / 3600;
    int mins = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;
    int msecs = static_cast<int>((seconds - totalSeconds) * 1000);

    //QString t = QString::number(hrs) + ":" + QString::number(mins) + ":" + QString::number(seconds);
    //return t;

    // Create a QTime object
    QTime time(hrs % 24, mins, secs, msecs); // % 24 to handle hours overflow

    // Format the QTime object as a string in the desired format (HH:mm:ss.zzz)
    return time.toString("HH:mm:ss");
}

QString Convert::SecondsToFormattedTime_mmss(const QString &secondsString)
{
    bool ok;
    int totalSeconds = secondsString.toInt(&ok);
    if (!ok) {
        return "00:00"; // Handle invalid input
    }

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // Format the string as mm:ss
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

QString Convert::SecondsToFormattedTime_dayhhmm(double timeInSeconds)
{
    int totalSeconds = static_cast<int>(timeInSeconds);
    int days = totalSeconds / 86400;
    int hours = (totalSeconds % 86400) / 3600;
    int minutes = (totalSeconds % 3600) / 60;

    return QString("%1 day %2:%3")
        .arg(days)
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'));
}

QString Convert::SecondsToFormattedTime_dayhhmmss(double timeInSeconds)
{
    int totalSeconds = static_cast<int>(timeInSeconds);
    int days = totalSeconds / 86400;
    int hours = (totalSeconds % 86400) / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    if (days == 0) {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    } else {
        return QString("%1 day %2:%3:%4")
            .arg(days)
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
}

double Convert::FormattedTime_dayhhmmssToSeconds(const QString &dayhhmmss)
{
    int days = 0, hours = 0, minutes = 0, seconds = 0;

    // Split the time into parts based on spaces
    QStringList parts = dayhhmmss.split(' ');

    // If there's a day part, it should be at the start
    if (parts.size() > 1 && parts[0].endsWith("day")) {
        bool ok;
        days = parts[0].split("day")[0].toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid day format.";
            return -1;  // Return an invalid value to indicate error
        }
        parts.removeFirst();  // Remove day part to handle the rest
    }

    // Process the time part
    if (!parts.isEmpty()) {
        QStringList timeParts = parts[0].split(':');

        // Depending on how many parts there are, parse accordingly
        switch (timeParts.size()) {
        case 3:  // hh:mm:ss
            hours = timeParts[0].toInt();
            minutes = timeParts[1].toInt();
            seconds = timeParts[2].toInt();
            break;
        case 2:  // mm:ss
            minutes = timeParts[0].toInt();
            seconds = timeParts[1].toInt();
            break;
        case 1:  // ss
            seconds = timeParts[0].toInt();
            break;
        default:
            qWarning() << "Invalid time format. Expected 'hh:mm:ss', 'mm:ss', or 'ss'.";
            return -1;  // Return an invalid value to indicate error
        }
    }

    // Calculate total seconds
    double totalSeconds = days * 86400 + hours * 3600 + minutes * 60 + seconds;

    return totalSeconds;
}

QString Convert::MinutesToFormattedTime_hhmm(double minutes)
{
    // Calculate hours and remaining minutes
    int totalMinutes = static_cast<int>(minutes);
    int hours = totalMinutes / 60;
    int mins = totalMinutes % 60;
    //int secs = static_cast<int>((minutes - totalMinutes) * 60);

    QString t = QString::number(hours) + ":" + QString::number(mins);
    return t;

    // Create a QTime object
    //QTime time(hours, mins, secs);

    // Format the QTime object as a string in the desired format (HH:mm:ss)
    //return time.toString("HH:mm:ss");
}

QString Convert::FormattedTime_mmssToSeconds(const QString &minutesSecondsString)
{
    QStringList parts = minutesSecondsString.split(':');
    if (parts.size() != 2) {
        return "0"; // Handle invalid input
    }

    bool ok;
    int minutes = parts[0].toInt(&ok);
    if (!ok) {
        return "0"; // Handle invalid minutes
    }

    int seconds = parts[1].toInt(&ok);
    if (!ok) {
        return "0"; // Handle invalid seconds
    }

    int totalSeconds = minutes * 60 + seconds;
    return QString::number(totalSeconds);
}

QString Convert::HoursToFormattedTime_hhmmss(double hours)
{
    // Calculate hours, minutes, and seconds
    int totalMinutes = static_cast<int>(hours * 60);
    int hrs = totalMinutes / 60;
    int mins = totalMinutes % 60;
    int secs = static_cast<int>((hours * 3600) - (hrs * 3600) - (mins * 60));

    // Create a QTime object
    QTime time(hrs % 24, mins, secs); // % 24 to handle hours overflow

    // Format the QTime object as a string in the desired format (HH:mm:ss)
    return time.toString("HH:mm:ss");
}

QString Convert::ToLocalDateTime(uint16_t DaysSince1970, double SecondsSinceMidnight, int16_t LocalOffset)
{

    // Calculate the base UTC date
    QDate baseDate(1970, 1, 1);
    QDateTime utcDateTime(baseDate.addDays(DaysSince1970), QTime(0, 0), Qt::UTC);

    // Add the seconds since midnight
    utcDateTime = utcDateTime.addSecs(SecondsSinceMidnight);

    // Apply the local time offset (in minutes)
    utcDateTime = utcDateTime.addSecs(LocalOffset * 60);

    // Convert to local time
    QDateTime localDateTime = utcDateTime.toLocalTime();

    // Return formatted local date and time
    //return localDateTime.toString(Qt::ISODate);
    return localDateTime.toString("M/d/yyyy H:mm:ss");
}

QString Convert::ToLocalDateTime(qint64 timeUTC, int localOffset)
{
    // Create a QDateTime from the given UTC time
    QDateTime utcDateTime = QDateTime::fromMSecsSinceEpoch(timeUTC, Qt::UTC);

    // Apply the local time offset (in minutes)
    QDateTime localDateTime = utcDateTime.addSecs(localOffset * 60);

    // Return formatted local date and time
    return localDateTime.toString("M/d/yyyy H:mm:ss");
}

/* Refactored removing the need to use helper class in the Convert class
QString Convert::Nmea0183TimeToLocalTimeFormatted(const QString &timeUTC, int localOffset)
{
    // NMEA 0183 UTC time is typically in "HHMMSS" or "HHMMSS.sss" format.
    if (timeUTC.length() < 6) {
        return QString("Invalid Time Format");
    }

    // Extract hours, minutes, and seconds from the string
    int hours = timeUTC.mid(0, 2).toInt();
    int minutes = timeUTC.mid(2, 2).toInt();
    int seconds = timeUTC.mid(4, 2).toInt();
    int milliseconds = 0;

    // Check for fractional seconds if provided
    if (timeUTC.length() > 6 && timeUTC[6] == '.') {
        milliseconds = timeUTC.mid(7).toInt();
    }

    // Create a QTime object for the UTC time
    QTime utcTime(hours, minutes, seconds, milliseconds);

    if (!utcTime.isValid()) {
        return QString("Invalid Time");
    }

    // Get the current date
    QDate currentDate = QDate::currentDate();

    // Combine date and time into a QDateTime object
    QDateTime utcDateTime(currentDate, utcTime, Qt::UTC);

    // Convert to local time using the offset
//    QDateTime localDateTime = utcDateTime.addSecs(localOffset * 3600);
    QDateTime localDateTime = utcDateTime.addSecs(localOffset * 60);

    // Check if Date is correct and if not fix
    QDateTime correctedDateTime = helper::correctDateTime(localDateTime);

    // Format the local date-time as a string
    //return localDateTime.toString("M/d/yyyy H:mm:ss");
    return correctedDateTime.toString("M/d/yyyy H:mm:ss");
}*/

// Added 8/29/2024 v0.5.3 to replace above method
QDateTime Convert::Nmea0183TimeToLocalDateTime(const QString &timeUTC, int localOffset)
{
    // Extract hours, minutes, and seconds from the string
    int hours = timeUTC.mid(0, 2).toInt();
    int minutes = timeUTC.mid(2, 2).toInt();
    int seconds = timeUTC.mid(4, 2).toInt();
    int milliseconds = 0;

    // Check for fractional seconds if provided
    if (timeUTC.length() > 6 && timeUTC[6] == '.') {
        milliseconds = timeUTC.mid(7).toInt();
    }

    // Create a QTime object for the UTC time
    QTime utcTime(hours, minutes, seconds, milliseconds);

    // Get the current date
    QDate currentDate = QDate::currentDate();

    // Combine date and time into a QDateTime object
    QDateTime utcDateTime(currentDate, utcTime, Qt::UTC);

    // Convert to local time using the offset
    return utcDateTime.addSecs(localOffset * 60);

}

QString Convert::SkTimeToLocalTimeFormatted(QString skTime, int localOffset)
{
    // Define the format that matches the input Signal K time format
    QString inputFormat = "ddd MMM d HH:mm:ss yyyy 'GMT'";

    // Parse the input time string into a QDateTime object
    QDateTime gmtDateTime = QDateTime::fromString(skTime, inputFormat);
    gmtDateTime.setTimeSpec(Qt::UTC);  // Explicitly set it as UTC

    if (!gmtDateTime.isValid()) {
        return QString(); // Return an empty string if parsing failed
    }

    // Apply the local offset (in seconds) to the GMT time
    QDateTime localDateTime = gmtDateTime.addSecs(localOffset);

    // Define the desired output format
    QString outputFormat = "M/d/yyyy H:mm:ss";

    // Return the formatted local time string
    return localDateTime.toString(outputFormat);
}

double Convert::LitersPerHourToLitersPerNauticalMile(double litersPerHour, double speedInKnots)
{
    if (speedInKnots == 0) {
        return 0;
    }
    return litersPerHour / speedInKnots;
}

double Convert::LitersPerHourToUSGallonsPerNauticalMile(double litersPerHour, double speedInKnots)
{
    double gallonsPerHour = litersPerHour / 3.78541;  // Convert LPH to GPH
    return gallonsPerHour / speedInKnots;  // GPH / Knots = Gallons per Nautical Mile
}

double Convert::LitersPerHourToMilesPerUSGallon(double litersPerHour, double speedInKnots)
{
    double gallonsPerHour = litersPerHour / 3.78541; // 3.78541 liters per US gallon
    double milesPerHour = speedInKnots * 1.15078; // 1.50779 knots to MPH
    return milesPerHour / gallonsPerHour;
}

double Convert::LitersPerHourToKilometersPerLiter(double litersPerHour, double speedInKnots)
{
    double kilometersPerHour = speedInKnots * KM_TO_NM;
    return kilometersPerHour / litersPerHour;
}

double Convert::LphToKmh(double fuelRateLh, double fuelEfficiencyKmL)
{
    if (fuelRateLh == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return fuelRateLh / fuelEfficiencyKmL;
}

double Convert::LitersPerNauticalMileToGallonsPerNauticalMile(double litersPerNauticalMile)
{
    return litersPerNauticalMile / LITERS_PER_US_GALLON;
}

double Convert::LitersPerNauticalMileToMilesPerUSGallon(double litersPerNauticalMile)
{
     return (1.15078 / litersPerNauticalMile) * 3.78541;
}

double Convert::LitersPerNauticalMileToNauticalMilesPerLiter(double litersPerNauticalMile)
{
    return 1.0 / litersPerNauticalMile;
}

double Convert::USGallonsPerNauticalMileToNauticalMilesPerLiter(double galPerNm)
{
    if (galPerNm <= 0) {
        qWarning() << "Invalid Gal/NM value: must be greater than zero.";
        return 0.0;
    }
    // Convert Gal/NM to L/NM, then to NM/L
    double litersPerNm = galPerNm * 3.78541;  // 3.78541 liters per US gallon
    return 1.0 / litersPerNm;
}

double Convert::MilesPerUSGallonToNauticalMilesPerLiter(double mpg)
{
    if (mpg <= 0) {
        qWarning() << "Invalid MPG value: must be greater than zero.";
        return 0.0;
    }
    // Convert MPG to nautical miles per gallon, then to nautical miles per liter
    double nmPerGal = mpg * 1.15078;
    return nmPerGal / 3.78541;
}

double Convert::NauticalMilesPerUSGallonToNauticalMilesPerLiter(double nmPerGal)
{
    if (nmPerGal <= 0) {
        qWarning() << "Invalid NM/Gal value: must be greater than zero.";
        return 0.0;
    }
    // Convert NM/Gal to NM/L
    return nmPerGal / 3.78541;
}

double Convert::LitersPerNauticalMileToNauticalMilesPerGallon(double litersPerNauticalMile)
{
    return (1.0 / litersPerNauticalMile) * LITERS_PER_US_GALLON;
}

double Convert::GallonsPerNauticalMileToLitersPerNauticalMile(double gallonsPerNauticalMile)
{
    return gallonsPerNauticalMile / LITERS_TO_GALLONS;
}

double Convert::LitersPerNauticalMileToKilometersPerLiter(double litersPerNauticalMile)
{
    return (NAUTICAL_MILES_PER_MILE * KILOMETERS_PER_MILE) / litersPerNauticalMile;
}

double Convert::USGallonsPerNauticalMileToKilometersPerLiter(double gallonsPerNauticalMile)
{
    if (gallonsPerNauticalMile <= 0) {
        qWarning() << "Invalid gallons/NM value: must be greater than zero.";
        return 0.0;
    }
    return (1.15078 * 1.60934) / (gallonsPerNauticalMile * 3.78541);
}

double Convert::MilesPerUSGallonToLitersPerNauticalMile(double milesPerGallon)
{
    return (1.15078 / milesPerGallon) * 3.78541;
}

double Convert::NauticalMilesPerLiterToLitersPerNauticalMile(double nmPerLiter)
{
    if (nmPerLiter <= 0) {
        qWarning() << "NauticalMilesPerLiterToLitersPerNauticalMile Invalid NM/L value: must be greater than zero.";
        return 0.0;
    }
    return 1.0 / nmPerLiter;
}

double Convert::NauticalMilesPerLiterToUSGallonsPerNauticalMile(double nmPerLiter)
{
    if (nmPerLiter <= 0) {
        qWarning() << "NauticalMilesPerLiterToGallonsPerNauticalMile Invalid NM/L value: must be greater than zero.";
        return 0.0;
    }
    double litersPerNauticalMile = 1.0 / nmPerLiter;
    return litersPerNauticalMile / 3.78541;
}

double Convert::NauticalMilesPerLiterToMilesPerUSGallon(double nmPerLiter)
{
    if (nmPerLiter <= 0) {
        qWarning() << "NauticalMilesPerLiterToMPG Invalid NM/L value: must be greater than zero.";
        return 0.0;
    }
    double nauticalMilesPerGallon = nmPerLiter * 3.78541;
    return nauticalMilesPerGallon * 1.15078;
}

double Convert::NauticalMilesPerLiterToNauticalMilesPerGallon(double nmPerLiter)
{
    if (nmPerLiter <= 0) {
        qWarning() << "NauticalMilesPerLiterToNauticalMilesPerGallon Invalid NM/L value: must be greater than zero.";
        return 0.0;
    }
    return nmPerLiter * LITERS_PER_US_GALLON;
}

double Convert::NauticalMilesPerUSGallonToLitersPerNauticalMile(double nauticalMilesPerGallon)
{
    return (1.0 / nauticalMilesPerGallon) * 3.78541;
}

double Convert::MilesPerUSGallonToKilometersPerLiter(double milesPerGallon)
{
    if (milesPerGallon <= 0) {
        qWarning() << "Invalid MPG value: must be greater than zero.";
        return 0.0;
    }
    return 1.60934 / milesPerGallon;
}

double Convert::KilometersPerLiterToLitersPerNauticalMile(double kilometersPerLiter)
{
    if (kilometersPerLiter <= 0) {
        qWarning() << "Invalid KM/L value: must be greater than zero.";
        return 0.0;
    }
    return (1.15078 * 1.60934) / kilometersPerLiter;
}

double Convert::KilometersPerLiterToMilesPerUSGallon(double kilometersPerLiter)
{
    if (kilometersPerLiter <= 0) {
        qWarning() << "Invalid KM/L value: must be greater than zero.";
        return 0.0;
    }
    return kilometersPerLiter * 0.621371 / 3.78541;
}

double Convert::CostPerLiterToCostPerGallon(double costPerLiter)
{
    return costPerLiter / LITERS_TO_GALLONS;
}

double Convert::CostPerGallonToCostPerLiter(double costPerGallon)
{
    return costPerGallon / LITERS_PER_US_GALLON;
}

double Convert::LkmToLnm(double value)
{
    return value * 0.539957;
}

double Convert::LkmToUSGalNm(double value)
{
    return value * 0.539957 * 0.264172;
}

double Convert::LkmToMPG(double value)
{
    if (value == 0.0) return 0.0;
    return 0.621371 / (value * 0.264172);
}

double Convert::LkmToNmL(double value)
{
    if (value == 0.0) return 0.0;
    return 1.0 / (value * 0.539957);
}

double Convert::LkmToNmUSGal(double value)
{
    if (value == 0.0) return 0.0;
    return 1.0 / (value * 0.539957 * 0.264172);
}

double Convert::LnmToLkm(double value)
{
    return value / 0.539957;
}

double Convert::USGalNmToLkm(double value)
{
    return value / (0.539957 * 0.264172);
}

double Convert::MPGToLkm(double value)
{
    if (value == 0.0) return 0.0;
    return MILES_TO_KM / (value / LITERS_TO_GALLONS);
}

double Convert::NmLToLkm(double value)
{
    if (value == 0.0) return 0.0;
    return 1.0 / (value * KM_TO_NM);
}

double Convert::NmGalToLkm(double value)
{
    if (value == 0.0) return 0.0;
    return 1.0 / (value * 0.539957 * 0.264172);
}

double Convert::m3sToLph(double m3s)
{
     return m3s * 1000 * 3600;
}

double Convert::lphToM3s(double lph)
{
    return lph / (1000.0 * 3600.0);
}

double Convert::M3sToLkm(double m3s, double velocityMs)
{
    if (velocityMs == 0.0) return 0.0;
    return (m3s * 1000.0) / velocityMs;
}

double Convert::LkmToM3s(double lkm, double velocityMs)
{
    if (velocityMs == 0.0) return 0.0;
    return (lkm * velocityMs) / 1000;
}

double Convert::GallonToCubicMeter(double gal)
{
    return gal * 0.0037854117840007;
}

double Convert::CubicMeterToGal(double cubicMeter)
{
    return cubicMeter * 264.1720523581;
}

QDateTime Convert::UnixTimestampToQDateTime(double timestamp)
{
    QDateTime origin = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);
    return origin.addSecs(static_cast<qint64>(timestamp));
}

double Convert::QDateTimeToUnixTimestamp(const QDateTime &date)
{
    QDateTime origin = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);
    qint64 diff = origin.secsTo(date.toUTC());
    return std::floor(static_cast<double>(diff));
}

double Convert::RadiansToDegrees(double radians)
{
    double degrees = (180 / 3.141592653589793) * radians;
    return degrees;
}

double Convert::DegreesToRadians(double degrees)
{
    double radians = degrees * (3.141592653589793 / 180);
    return radians;
}

double Convert::MetersToNauticalMiles(double meters)
{
    return meters * 0.00053995680;
}

double Convert::NauticalMilesToMeters(double nm)
{
    double miles = nm * METERS_PER_NAUTICAL_MILE;
    return miles;
}

double Convert::NauticalMilesToMiles(double nm)
{
    double miles = nm * 1.15078;
    return miles;
}

double Convert::NauticalMilesToKilometers(double nm)
{
    double km = nm * 1.852;
    return km;
}

double Convert::KilometersToNauticalMiles(double kilometers)
{
    return kilometers / 1.852;
}

double Convert::MilesToNauticalMiles(double miles)
{
    return miles / 1.15078;
}

double Convert::MilesToMeters(double miles)
{
    double meters = miles * 1609.344;
    return meters;
}

double Convert::MetersToMiles(double meters)
{
    double miles = meters * 0.00062137119;
    return miles;
}

double Convert::FeetToDecimeters(double feet)
{
    return feet * 3.048;
}

double Convert::MagneticToTrue(double degrees, double variation)
{
    double magentic = degrees + variation;
    // Normalize the heading to be within 0-360 degrees
    if (magentic > 360) {
        magentic -= 360;
    } else if (magentic < 0) {
        magentic += magentic;
    }
    return magentic;
}

double Convert::MagneticToTrueHeading(double magneticHeading, double magneticVariation, bool isVariationEast)
{
    double trueHeading;

    if (isVariationEast) {
        trueHeading = magneticHeading - magneticVariation;
    } else {
        trueHeading = magneticHeading + magneticVariation;
    }

    // Normalize the heading to be within 0-360 degrees
    if (trueHeading < 0) {
        trueHeading += 360.0;
    } else if (trueHeading >= 360.0) {
        trueHeading -= 360.0;
    }

    return trueHeading;
}

double Convert::TrueToMagentic(double degrees, double variation)
{
    double tr = degrees - variation;
    // Normalize the heading to be within 0-360 degrees
    if (tr > 360) {
        tr -= 360;
    } else if (tr < 0) {
        tr += degrees;
    }
    return tr;
}

double Convert::TrueToMagneticHeading(double trueHeading, double magneticVariation, bool isVariationEast)
{
    double magneticHeading;

    if (isVariationEast) {
        magneticHeading = trueHeading - magneticVariation;
    } else {
        magneticHeading = trueHeading + magneticVariation;
    }

    // Normalize the heading to be within 0-360 degrees
    if (magneticHeading < 0) {
        magneticHeading += 360.0;
    } else if (magneticHeading >= 360) {
        magneticHeading -= 360.0;
    }

    return magneticHeading;
}

double Convert::PercentageToLiters(double percentage, double tankCapacityLiters)
{
    return (percentage / 100.0) * tankCapacityLiters;
}

double Convert::LitersToUSGallons(double liters)
{
    return liters * 0.264172;
}

double Convert::USGallonsToLiters(double usGallons)
{
    return usGallons * 3.78541;
}

unsigned char Convert::QStringHexToUInt(QString str)
{
    // Remove "0x" prefix if present
    if (str.startsWith("0x")) {
        str = str.mid(2);
    }

    bool ok;
    unsigned int intValue = str.toUInt(&ok, 16);

    // Check if the conversion was successful
    if (ok) {
        // Cast to unsigned char
        unsigned char ucharValue = static_cast<unsigned char>(intValue);
        return ucharValue;
    } else {
        qWarning() << "Conversion QStringToUInt failed!";
        return 0;
    }
}

double Convert::MetersPerSecondToKnots(double ms)
{
    return ms *= 1.94384;
}

double Convert::KnotsToMetersPerSecond(double knots)
{
    return knots * 0.514444;
}

double Convert::MetersPerSecondToKmh(double ms)
{
    return ms *= 3.6;
}

double Convert::MetersPerSecondToMph(double ms)
{
    return ms *= 2.23694;
}

double Convert::MphToMetersPerSecond(double mph)
{
    return mph * 0.44704;
}

double Convert::MetersToFeet(double meters)
{
    return meters * 3.28084;
}

double Convert::MetersToFathoms(double meters)
{
    return meters * 0.546807;
}

double Convert::FeetToMeters(double feet)
{
    return feet * 0.3048;
}

double Convert::FathomsToMeters(double fathoms)
{
    return fathoms * 1.8288;
}

// Outdrive trim percentage to degrees where 0% = -5 deg and 100% = 45 deg
double Convert::TrimPercentageToDegrees(double percentage)
{
    return -5 + 0.5 * percentage;
}

// Outdrive trim percentage to degrees where -5 deg = 0% and 45 deg = 100%
double Convert::DegreesToTrimPercentage(double degrees)
{
    return 2 * (degrees + 5);
}

// Convert decimal degrees to DMM with direction
QString Convert::DecimalToDMM(double decimalDegrees, bool isLatitude)
{
    int degrees = static_cast<int>(fabs(decimalDegrees));
    double minutes = (fabs(decimalDegrees) - degrees) * 60.0;
    char direction = isLatitude ? (decimalDegrees >= 0 ? 'N' : 'S') : (decimalDegrees >= 0 ? 'E' : 'W');

    return QString("%1° %2' %3").arg(degrees).arg(minutes, 0, 'f', 3).arg(direction);
}

// Convert decimal degrees to DMS with direction
QString Convert::DecimalToDMS(double decimalDegrees, bool isLatitude)
{
    int degrees = static_cast<int>(fabs(decimalDegrees));
    double fractional = (fabs(decimalDegrees) - degrees) * 60.0;
    int minutes = static_cast<int>(fractional);
    double seconds = (fractional - minutes) * 60.0;
    char direction = isLatitude ? (decimalDegrees >= 0 ? 'N' : 'S') : (decimalDegrees >= 0 ? 'E' : 'W');

    return QString("%1° %2' %3\" %4").arg(degrees).arg(minutes).arg(seconds, 0, 'f', 2).arg(direction);
}

// Convert DMM to decimal degrees
double Convert::DmmToDecimal(int degrees, double minutes, char direction)
{
    double decimalDegrees = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W') {
        decimalDegrees = -decimalDegrees;
    }
    return decimalDegrees;
}

// Convert DMS to decimal degrees
double Convert::DmsToDecimal(int degrees, int minutes, double seconds, char direction)
{
    double decimalDegrees = degrees + (minutes / 60.0) + (seconds / 3600.0);
    if (direction == 'S' || direction == 'W') {
        decimalDegrees = -decimalDegrees;
    }
    return decimalDegrees;
}

double Convert::Convert0183DegreesToDecimal(const QString &coordinate, const QString &direction)
{
    bool ok;
    int degLength = (direction == "N" || direction == "S") ? 2 : 3;
    double degrees = coordinate.left(degLength).toDouble(&ok);
    if (!ok) return 0;

    double minutes = coordinate.mid(degLength).toDouble(&ok);
    if (!ok) return 0;

    double decimalDegrees = degrees + (minutes / 60.0);

    if (direction == "S" || direction == "W") {
        decimalDegrees = -decimalDegrees;
    }

    return decimalDegrees;

}

double Convert::Convert0183AltitudeToMeters(double altitude, const QString &unit)
{
    if (unit == "M") {
        return altitude;
    } else if (unit == "F") {
        return altitude * 0.3048;
    }
    return altitude; // Default to meters if the unit is unrecognized
}

double Convert::Convert0183GeoidHeightToMeters(double height, const QString &unit)
{
    if (unit == "M") {
        return height;
    } else if (unit == "F") {
        return height * 0.3048;
    }
    return height; // Default to meters if the unit is unrecognized
}

double Convert::Convert0183TimeUTCToSeconds(const QString &timeUTC)
{
    if (timeUTC.length() < 6) {
        return 0;
    }

    int hours = timeUTC.mid(0, 2).toInt();
    int minutes = timeUTC.mid(2, 2).toInt();
    double seconds = timeUTC.mid(4).toDouble();

    return hours * 3600 + minutes * 60 + seconds;
}
