#include <QtCore/QRegExp>
#include <QDebug>
#include <cmath>
#include "utils.h"

#define PI 3.14159265
#define EARTH_RADIUS 6371

bool grid_validate(QString grid) {
    QRegExp regex("^[A-Za-z]{2}[0-9]{2}([A-Za-z]{2})?$");
    return regex.exactMatch(grid);
}

bool grid_to_coord(QString grid, double &lat, double &lon) {
    if (!grid_validate(grid)) return false;

    grid = grid.toUpper();

    lon = (grid.at(0).toLatin1() - 'A') * 20 - 180;
    lat = (grid.at(1).toLatin1() - 'A') * 10 - 90;

    lon += (grid.at(2).toLatin1() - '0') * 2;
    lat += (grid.at(3).toLatin1() - '0') * 1;

    if (grid.size() >= 6) {
        lon += (grid.at(4).toLatin1() - 'A') * (5/60.0);
        lat += (grid.at(5).toLatin1() - 'A') * (2.5/60.0);

        // move to the center
        lon += 2.5/60;
        lat += 1.25/60;
    }
    else {
        // move to the center
        lon += 1;
        lat += 0.5;
    }

    return true;
}

bool grid_distance(QString gridA, QString gridB, double &distance) {
    double latA, lonA, latB, lonB;
    bool validA = grid_to_coord(gridA, latA, lonA);
    bool validB = grid_to_coord(gridB, latB, lonB);

    if (!validA || !validB) return false;

    double dLat = (latB-latA)*PI/180;
    double dLon = (lonB-lonA)*PI/180;
    latA = latA*PI/180;
    latB = latA*PI/180;

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(latA) * cos(latB);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    distance = EARTH_RADIUS * c;

    return true;
}

bool grid_bearing(QString gridA, QString gridB, double &distance) {

}
