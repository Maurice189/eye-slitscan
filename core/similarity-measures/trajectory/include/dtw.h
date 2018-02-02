#ifndef DTW_H
#define DTW_H

#include <limits>
#include <cmath>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include "util.h"
#include "../../../importer/eye-tracker-output/include/tobii_importer.h"

struct DTWResult {
   double normalizedDistance;

   std::pair<double, double> mean01;
   std::pair<double, double> mean02;

   std::pair<double, double> std01;
   std::pair<double, double> std02;
};

inline double euclDist(double x, double y);
double calcGazePointDistance(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2);
double distanceDTW(const struct Trajectory& trP1,
                   const struct Trajectory& trP2, bool normalize);
double distanceDTW(const struct Trajectory& trP1,
                   const struct Trajectory& trP2);
double scoreDTW(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                std::pair<int, int> screenDim,
                bool normalize = true);
#endif // DTW_H
