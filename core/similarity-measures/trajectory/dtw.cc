#include "dtw.h"

inline double euclDist(double x, double y)
{
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

double calcGazePointDistance(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2)
{
    double distX = p1.first - p2.first;
    double distY = p1.second - p2.second;

    return euclDist(distX, distY);
}

double scoreDTW(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                std::pair<int, int> screenDim,
                bool normalize)
{
    double maxDistance = euclDist(screenDim.first, screenDim.second) * std::max(trP1.points.size(), trP2.points.size());
    double dtwDistance;

    if (trP1.points.empty() && trP2.points.empty()) {
        throw std::invalid_argument("scoreDTW ::: both sequences are empty");
    }

    if (normalize) {
        const struct NormalizedTrajectory nt1 = normalizeTrajectory(trP1);
        const struct NormalizedTrajectory nt2 = normalizeTrajectory(trP2);

        double distN1 = euclDist(zNormalize(screenDim.first, nt1.meanX, nt1.stdX) + zNormalize(0, nt2.meanX, nt2.meanX),
                                 zNormalize(screenDim.second, nt1.meanY, nt1.stdY) + zNormalize(0, nt2.meanY, nt2.stdY));

        double distN2 = euclDist(zNormalize(screenDim.first, nt2.meanX, nt2.stdX) + zNormalize(0, nt1.meanX, nt1.meanX),
                                 zNormalize(screenDim.second, nt2.meanY, nt2.stdY) + zNormalize(0, nt1.meanY, nt1.stdY));

        maxDistance = std::max(distN1, distN2) * std::max(nt1.points.size(), nt2.points.size());
        dtwDistance = distanceDTW(nt1, nt2);

    } else {
        dtwDistance = distanceDTW(trP1, trP2);
    }

    return maxDistance > 0. ? 1. - (dtwDistance / maxDistance) : -1.;
}

double distanceDTW(const struct Trajectory& trP1,
                   const struct Trajectory& trP2,
                   bool normalize)
{
    if (normalize) {
        const struct NormalizedTrajectory nt1 = normalizeTrajectory(trP1);
        const struct NormalizedTrajectory nt2 = normalizeTrajectory(trP2);

        return distanceDTW(nt1, nt2);
    }
    return distanceDTW(trP1, trP2);
}

double distanceDTW(const struct Trajectory& trP1,
                   const struct Trajectory& trP2)
{

    const int n = trP1.points.size();
    const int m = trP2.points.size();

    if (n == 0 || m == 0) {
        // In that case, cost is undefined,
        // we set it to zero, which results in zero distance.
        return 0.0;
    }

    double *dist = new double[(n+1)*(m+1)];

    for (int i = 1; i < n+1; ++i) {
        dist[i] = std::numeric_limits<double>::max();
    }
    for (int j = 1; j < m+1; ++j) {
        dist[j*(n+1)] = std::numeric_limits<double>::max();
    }

    dist[0] = 0.0;

    for (int i = 1; i < n+1; ++i) {
        for (int j = 1; j < m+1; ++j) {
            double cost = calcGazePointDistance(trP1.points[i-1], trP2.points[j-1]);
            // Insertion , Deletion, Match
            dist[i+j*(n+1)] = cost + std::min({dist[(i-1)+j*(n+1)],
                                               dist[i+(j-1)*(n+1)],
                                               dist[(i-1)+(j-1)*(n+1)]
                                              });
        }
    }

    double dtwDistance = dist[n+m*(n+1)];

    delete[] dist;
    return dtwDistance;
}

