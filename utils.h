#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

bool grid_validate(QString grid);
bool grid_to_coord(QString grid, double &lat, double &lon);
double coord_distance(double latA, double lonA, double latB, double lonB);
int coord_bearing(double latA, double lonA, double latB, double lonB);

#endif // UTILS_H
