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

#include "autopilotsimulator.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

AutoPilotSimulator::AutoPilotSimulator(QObject *parent)
    : QObject(parent), currentIndex(0), speed(5), reverseRoute(false), forwardDirection(true)
{
}

AutoPilotSimulator::~AutoPilotSimulator()
{
    if (updateTimer.isActive()) {
        updateTimer.stop();
    }
    stop();
    disconnect();
}

bool AutoPilotSimulator::loadRoute(const QString &filePath)
{
    if (filePath.endsWith(".kml", Qt::CaseInsensitive)) {
        return parseKMLFile(filePath);
    } else if (filePath.endsWith(".gpx", Qt::CaseInsensitive)) {
        return parseGPXFile(filePath);
    } else {
        qWarning() << "Unsupported file format:" << filePath;
        return false;
    }
}

void AutoPilotSimulator::start()
{
    if (route.isEmpty()) {
        qWarning() << "Route is empty. Load a route before starting.";
        return;
    }

    if (!updateTimer.isActive()) {
        connect(&updateTimer, &QTimer::timeout, this, &AutoPilotSimulator::calculateNextCoordinate);
    }

    travelTimer.start();
    totalDistanceTraveled = 0.0;
    currentIndex = 0;
    lastIndex = -1;
    forwardDirection = true;
    emitNextWaypoint();
    updateTimer.start(1000); // Update every second
    emit RunningStateChanged(true);
}

void AutoPilotSimulator::stop()
{
    updateTimer.stop();
    emit RunningStateChanged(false);
}

double AutoPilotSimulator::getSpeed() const
{
    return speed;
}

void AutoPilotSimulator::setSpeed(double newSpeed)
{
    speed = newSpeed;
}

bool AutoPilotSimulator::parseKMLFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file:" << filePath;
        emit routeLoaded(false);
        return false;
    }

    QXmlStreamReader xml(&file);
    route.clear(); // Clear any existing route data

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Parse only if we are at a start element
        if (xml.isStartElement()) {
            if (xml.name() == QStringLiteral("Placemark")) {
                QString waypointName;
                QGeoCoordinate coordinate;

                while (!(xml.isEndElement() && xml.name() == QStringLiteral("Placemark"))) {
                    xml.readNext();

                    if (xml.isStartElement()) {
                        if (xml.name() == QStringLiteral("name")) {
                            waypointName = xml.readElementText();
                        } else if (xml.name() == QStringLiteral("coordinates")) {
                            QStringList coordList = xml.readElementText().trimmed().split(',');

                            if (coordList.size() >= 2) {
                                double longitude = coordList[0].toDouble();
                                double latitude = coordList[1].toDouble();
                                coordinate = QGeoCoordinate(latitude, longitude);
                            }
                        }
                    }
                }

                if (coordinate.isValid()) {
                    Waypoint waypoint;
                    waypoint.coordinate = coordinate;
                    waypoint.name = waypointName;
                    route.append(waypoint);
                    //qDebug() << "Loaded waypoint:" << waypoint.name
                    //         << coordinate.latitude() << coordinate.longitude();
                }
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "Error parsing KML file:" << xml.errorString();
        emit routeLoaded(false);
        return false;
    }

    //qDebug() << "Total waypoints loaded:" << route.size();
    emit routeLoaded(true);
    return true;
}

bool AutoPilotSimulator::parseGPXFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    QXmlStreamReader xml(&file);

    route.clear();

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString elementName = xml.name().toString();

            if (elementName == QStringLiteral("trkpt")) {
                // Extract the latitude and longitude from the trkpt attributes
                double lat = xml.attributes().value("lat").toDouble();
                double lon = xml.attributes().value("lon").toDouble();
                Waypoint waypoint;
                waypoint.coordinate = QGeoCoordinate(lat, lon);

                // Look for any sub-elements like <name>
                while (!(xml.isEndElement() && xml.name() == QStringLiteral("trkpt"))) {
                    xml.readNext();

                    if (xml.isStartElement()) {
                        if (xml.name() == QStringLiteral("name")) {
                            waypoint.name = xml.readElementText();
                        }
                    }
                }

                route.append(waypoint);
                //qDebug() << "Loaded waypoint:" << waypoint.coordinate.latitude() << waypoint.coordinate.longitude();
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML Error:" << xml.errorString();
        return false;
    }

    file.close();

    //qDebug() << "Total waypoints loaded:" << route.size();
    return !route.isEmpty();
}

void AutoPilotSimulator::setReverseRoute(bool reverse)
{
    reverseRoute = reverse;
}

void AutoPilotSimulator::calculateNextCoordinate()
{
    static double accumulatedDistance = 0;  // Track distance covered towards the next waypoint

    if (currentIndex < 0 || currentIndex >= route.size()) {
        stop();
        emit routeCompleted();
        emit RunningStateChanged(false);
        //qDebug() << "Route completed or index out of bounds.";
        return;
    }

    const QGeoCoordinate &current = route[currentIndex].coordinate;
    QGeoCoordinate next;

    //qDebug() << "Current Index:" << currentIndex
    //         << "Current Coordinate:" << current.latitude() << current.longitude();

    // Determine the next waypoint based on direction
    if (forwardDirection) {
        if (currentIndex < route.size() - 1) {
            next = route[currentIndex + 1].coordinate;
            if (currentIndex != lastIndex) {
                QString msg = "Traveling forward to Wp " + QString::number(currentIndex + 1) + " of " + QString::number(route.size());
                emit statusMessage(msg);
                lastIndex = currentIndex;
            }
        } else if (reverseRoute) {
            forwardDirection = false;
            currentIndex--;  // Reverse direction
            next = route[currentIndex].coordinate;
            if (currentIndex != lastIndex) {
                QString msg = "Traveling back to Wp " + QString::number(currentIndex) + " of " + QString::number(route.size());
                emit statusMessage(msg);
                lastIndex = currentIndex;
            }
            emitNextWaypoint();
        } else {
            stop();
            emit routeCompleted();
            emit RunningStateChanged(false);
            return;
        }
    } else {
        if (currentIndex > 0) {
            next = route[currentIndex - 1].coordinate;
        } else {
            forwardDirection = true;
            currentIndex++;
            next = route[currentIndex].coordinate;
            emitNextWaypoint();
        }
    }

    // Calculate the cross-track error (XTE)
    double xteMeters = calculateXTE(current, waypointPosition);
    emit XTEChanged(xteMeters);

    double distanceToNext = current.distanceTo(next);
    //qDebug() << "Speed:" << speed
    //         << "Next Coordinate:" << next.latitude() << next.longitude()
    //         << "Distance to Next:" << distanceToNext;

    vesselWpBearing.WaypointClosingVelocity = speed;
    if (speed > 0) {
        double distancePerInterval = speed * updateTimer.interval() / 1000.0;
        accumulatedDistance += distancePerInterval;
        //qDebug() << "Accumulated Distance:" << accumulatedDistance;

        QGeoCoordinate interpolated = current.atDistanceAndAzimuth(accumulatedDistance, current.azimuthTo(next));
        double currentDistanceToNext = interpolated.distanceTo(next);
        Q_UNUSED(currentDistanceToNext);
        // Update total distance traveled
        totalDistanceTraveled += distancePerInterval;

        // Convert total distance to nautical miles and emit
        double distanceInNauticalMiles = totalDistanceTraveled / 1852.0; // 1 nautical mile = 1852 meters
        emit distanceTraveledNM(distanceInNauticalMiles);
        //qDebug() << "Total Distance Traveled:" << totalDistanceTraveled << "meters"
        //         << distanceInNauticalMiles << "nautical miles";

        emitDistanceAndTime(interpolated, next); // Pass interpolated position for accurate calculations
        emit timeTraveledSeconds(travelTimer.elapsed() / 1000.0);

        if (accumulatedDistance >= distanceToNext) {
            // Move to the next waypoint
            if (forwardDirection) {
                currentIndex++;
            //    qDebug() << "Incremented currentIndex to:" << currentIndex;
            } else {
                currentIndex--;
            //    qDebug() << "Decremented currentIndex to:" << currentIndex;
            }
            accumulatedDistance = 0;  // Reset accumulated distance
            if (next.isValid()) {
                emit coordinateUpdated(next);
                emit headingUpdated(calculateHeading(current, next));
                emitNextWaypoint();
            }
        } else {
            if (interpolated.isValid()) {
                // Update interpolated position
                emit coordinateUpdated(interpolated);
                emit headingUpdated(calculateHeading(current, interpolated));
                //qDebug() << "Interpolated Coordinate:" << interpolated.latitude() << interpolated.longitude();
            }
        }
    } else {
        //qWarning() << "Speed is zero; simulator is not moving."; // Okay for speed to be 0 for pumping out
    }

    emit VesselWpBearingChanged(vesselWpBearing);
}

void AutoPilotSimulator::emitNextWaypoint()
{
    if (forwardDirection && currentIndex < route.size() - 1) {
        vesselWpBearing.WaypointName = route[currentIndex + 1].name;
        vesselWpBearing.DestinationLatitude = route[currentIndex + 1].coordinate.latitude();
        vesselWpBearing.DestinationLongitude = route[currentIndex + 1].coordinate.longitude();
        emit nextWaypoint(route[currentIndex + 1].name);
        waypointPosition = route[currentIndex + 1].coordinate;
        //qDebug() << "Next Waypoint:" << route[currentIndex + 1].name;
    } else if (!forwardDirection && currentIndex > 0) {
        vesselWpBearing.WaypointName = route[currentIndex - 1].name;
        vesselWpBearing.DestinationLatitude = route[currentIndex - 1].coordinate.latitude();
        vesselWpBearing.DestinationLongitude = route[currentIndex - 1].coordinate.longitude();
        emit nextWaypoint(route[currentIndex - 1].name);
        waypointPosition = route[currentIndex - 1].coordinate;
        //qDebug() << "Next Waypoint:" << route[currentIndex - 1].name;
    } else {
        //qDebug() << "No next waypoint available.";
    }
}

double AutoPilotSimulator::calculateHeading(const QGeoCoordinate &from, const QGeoCoordinate &to)
{
    double azimuthDegrees = from.azimuthTo(to); // Calculate azimuth in degrees
    double azimuthRadians = qDegreesToRadians(azimuthDegrees); // Convert to radians
    vesselWpBearing.BearingPositionToDestinationWaypoint = azimuthRadians;
    return azimuthRadians;
}

void AutoPilotSimulator::emitDistanceAndTime(const QGeoCoordinate &current, const QGeoCoordinate &next)
{
    double distanceMeters = current.distanceTo(next);
    double timeSeconds = (speed > 0) ? (distanceMeters / speed) : std::numeric_limits<double>::infinity();

    //qDebug() << "Emitting Distance to Next Waypoint:" << distanceMeters << "meters";
    //qDebug() << "Emitting Time to Next Waypoint:" << timeSeconds << "seconds";

    vesselWpBearing.DistanceToWaypointM = distanceMeters;
    vesselWpBearing.EtaTimeS = timeSeconds;
    emit distanceToNextWaypoint(distanceMeters);
    emit timeToNextWaypoint(timeSeconds);
}

void AutoPilotSimulator::updateLatLong(double latitude, double longitude)
{
    latitude = Convert::DegreesToRadians(latitude);
    longitude = Convert::DegreesToRadians(longitude);
    emit LatLongChanged(latitude, longitude);
}

double AutoPilotSimulator::calculateXTE(const QGeoCoordinate &currentPosition,
                                        const QGeoCoordinate &waypointPosition)
{
    // Calculate the azimuths (bearing angles) from the previous waypoint to the current position and to the next waypoint
    if (currentIndex == 0 || currentIndex >= route.size() - 1) {
        return 0.0; // No previous waypoint to calculate XTE against
    }

    const QGeoCoordinate &previousWaypoint = route[currentIndex - 1].coordinate;

    // Calculate the bearing from the previous waypoint to the next waypoint (track bearing)
    double trackBearing = previousWaypoint.azimuthTo(waypointPosition);

    // Calculate the bearing from the previous waypoint to the current position
    double bearingToCurrent = previousWaypoint.azimuthTo(currentPosition);

    // Calculate the distance from the previous waypoint to the current position
    double distanceToCurrent = previousWaypoint.distanceTo(currentPosition);

    // The cross-track error (XTE) is the perpendicular distance from the current position to the intended track
    double angleDifference = bearingToCurrent - trackBearing;
    double xteMeters = distanceToCurrent * sin(qDegreesToRadians(angleDifference));

    vesselWpBearing.Xte = xteMeters;
    return xteMeters;
}
