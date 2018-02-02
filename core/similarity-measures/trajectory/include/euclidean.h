#ifndef EUCLIDEAN_H
#define EUCLIDEAN_H

#include <limits>
#include <cmath>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include "util.h"
#include "../../../importer/eye-tracker-output/include/tobii_importer.h"

int euclideanDistance(const struct Trajectory& trP1,
                      const struct Trajectory& trP2);
double calcGazePointDistanceE(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2);

#endif // EUCLIDEAN_H
