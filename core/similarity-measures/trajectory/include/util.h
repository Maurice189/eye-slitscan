#ifndef TRAJECTORY_UTIL_H
#define TRAJECTORY_UTIL_H

#include "../../../importer/eye-tracker-output/include/tobii_importer.h"
#include <cmath>
#include <vector>
#include <utility>
#include <stdexcept>

struct Trajectory {
    std::vector<std::pair<double, double>> points;
};

struct NormalizedTrajectory : Trajectory {
    double stdX;
    double stdY;
    double meanX;
    double meanY;
};


struct Trajectory createTrajectoryFromFixationData(const std::vector<struct TobiiExportData> &in);
struct Trajectory createTrajectoryFromGazeData(const std::vector<struct TobiiExportData> &in);

struct NormalizedTrajectory normalizeTrajectory(const struct Trajectory& in);
inline double zNormalize(double x, double mean, double stdDev)
{
    return stdDev > 0. ? (x - mean) / stdDev : 0.;
}
#endif // TRAJECTORY_UTIL_H
