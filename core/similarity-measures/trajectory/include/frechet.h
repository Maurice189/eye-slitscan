#ifndef FRECHET_H
#define FRECHET_H

#include "util.h"
#include "../../../importer/eye-tracker-output/include/tobii_importer.h"

#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

double discreteFrechetDistance(const struct Trajectory& trP1,
                               const struct Trajectory& trP2);

double rc(const struct Trajectory& trP1,
                         const struct Trajectory& trP2,
                         std::vector<std::vector<double>> &table,
                         int i, int j);

double euclDist(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2);

#endif // FRECHET_H
