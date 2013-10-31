#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

bool grid_validate(QString grid);
bool grid_to_coord(QString grid, double &lat, double &lon);
bool grid_distance(QString gridA, QString gridB, double &distance);
bool grid_bearing(QString gridA, QString gridB, double &distance);

#endif // UTILS_H
