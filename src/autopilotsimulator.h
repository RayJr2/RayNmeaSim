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

#ifndef AUTOPILOTSIMULATOR_H
#define AUTOPILOTSIMULATOR_H

#include <QElapsedTimer>
#include <QGeoCoordinate>
#include <QObject>
#include <QShowEvent>
#include <QString>
#include <QTimer>
#include <QVector>
#include "convert.h"
//#include "nmea0183handler.h" // Access to VesselWpBearing struct

struct Waypoint {
    QGeoCoordinate coordinate;
    QString name;
};

class AutoPilotSimulator : public QObject
{
    Q_OBJECT

public:
    explicit AutoPilotSimulator(QObject *parent = nullptr);
    ~AutoPilotSimulator();

    bool loadRoute(const QString &filePath);
    void start();
    void stop();
    double getSpeed() const;

    void setReverseRoute(bool reverse);

signals:
    void coordinateUpdated(const QGeoCoordinate &coordinate);
    void LatLongChanged(double latitude, double longitude);
    void nextWaypoint(const QString &name);
    void routeCompleted();
    void routeLoaded(bool loaded);
    void headingUpdated(double headingRadians);
    void statusMessage(const QString &message);
    void distanceToNextWaypoint(double distanceMeters); // New signal for distance in meters
    void timeToNextWaypoint(double timeSeconds);
    void RunningStateChanged(bool state);
    void distanceTraveledNM(double distanceNauticalMiles);
    void timeTraveledSeconds(double totalTimeSeconds);
    void XTEChanged(double XTE);
    // TODO Refactor to use VesselWpBearing struct and removed redundant signals
    void VesselWpBearingChanged(VesselWpBearing vesselWpBearing);

public slots:
    void setSpeed(double speed);

private:
    QVector<Waypoint> route;
    int currentIndex;
    int lastIndex = -1;
    double speed; // in meters per second
    QTimer updateTimer;
    QElapsedTimer travelTimer;
    bool reverseRoute;
    bool forwardDirection;
    QGeoCoordinate waypointPosition;
    VesselWpBearing vesselWpBearing = VesselWpBearing();

    void loadSettingGeometry();
    void saveSettingGeometry();
    bool parseKMLFile(const QString &filePath);
    bool parseGPXFile(const QString &filePath);
    void calculateNextCoordinate();
    void emitNextWaypoint();
    double calculateHeading(const QGeoCoordinate &from, const QGeoCoordinate &to);
    void emitDistanceAndTime(const QGeoCoordinate &current, const QGeoCoordinate &next);
    void updateLatLong(double latitude, double longitude);
    double totalDistanceTraveled = 0.0; // in meters
    double calculateXTE(const QGeoCoordinate &currentPosition,
                        const QGeoCoordinate &waypointPosition);
};

#endif // AUTOPILOTSIMULATOR_H
