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

#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>
#include "backgrounditem.h"

class Compass : public QWidget {
    Q_OBJECT

public:
    explicit Compass(QWidget *parent = nullptr);
    void setHeading(float heading);
    void setWaypointHeading(float heading);
    void enableWaypointNeedle(bool enable);
    BackgroundItem* addBackground(int radius);
    void setDegreeTextColor(const QColor &color);
    void setValueTextColor(const QColor &color);
    void setValueSuffix(QString suffix);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_heading;
    float m_waypointHeading;
    bool m_waypointNeedleEnabled;
    QVector<BackgroundItem*> m_backgrounds;
    QColor m_degreeTextColor;
    QColor m_valueTextColor;
    QString m_valueSuffix = "˚T";
};

#endif // COMPASS_H
