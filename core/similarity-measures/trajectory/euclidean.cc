#include "euclidean.h"

int euclideanDistance(const struct Trajectory& trP1,
                      const struct Trajectory& trP2)
{
    const int n = trP1.points.size();
    const int m = trP2.points.size();

    assert(n == m);
    double distance = 0;

    for (int i = 0; i < n; ++i) {
        distance += calcGazePointDistanceE(trP1.points[i], trP2.points[i]);
    }

    return std::sqrt(distance);
}

double calcGazePointDistanceE(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2)
{
    double distX = p1.first - p2.first;
    double distY = p1.second - p2.second;

    return std::pow(distX, 2) + std::pow(distY, 2);
}
