#include "edr.h"

double scoreEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                bool normalize,
                double matchingThreshold)
{
    double edrDistance;
    double maxDistance = std::max(trP1.points.size(), trP2.points.size());

    if (maxDistance == 0) {
        throw std::invalid_argument("scoreEDR ::: both sequences are empty");
    }

    if (normalize) {
        const struct NormalizedTrajectory nt1 = normalizeTrajectory(trP1);
        const struct NormalizedTrajectory nt2 = normalizeTrajectory(trP2);

        edrDistance = distanceEDR(nt1, nt2, matchingThreshold);
    } else {
        edrDistance = distanceEDR(trP1, trP2, matchingThreshold);
    }

    return maxDistance > 0. ? 1 - (edrDistance / maxDistance) : -1.;
}

int distanceEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                double matchingThreshold,
                bool normalize)
{
    if (normalize) {
        const struct NormalizedTrajectory nt1 = normalizeTrajectory(trP1);
        const struct NormalizedTrajectory nt2 = normalizeTrajectory(trP2);

        return distanceEDR(nt1, nt2, matchingThreshold);
    }

    return distanceEDR(trP1, trP2, matchingThreshold);
}

int distanceEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                double matchingThreshold)
{
    const int n = trP1.points.size();
    const int m = trP2.points.size();

    if (n == 0 || m == 0) {
        // In that case, cost is undefined,
        // we set it to zero, which results in zero distance.
        return std::max(n, m);
    }

    // Element (i,j) denotes the distance needed
    // to transform one trajectory subsequence
    // (with points 0 to i) to another trajectory subsequence
    // (with points 0 to j)

    int *dist = new int[(n+1)*(m+1)];

    for (int i = 1; i < n+1; ++i) {
        dist[i] = i;
    }
    for (int j = 1; j < m+1; ++j) {
        dist[j*(n+1)] = j;
    }

    dist[0] = 0.;

    for (int i = 1; i < n+1; ++i) {
        for (int j = 1; j < m+1; ++j) {
            int cost = match(trP1.points[i], trP2.points[j], matchingThreshold) ? 0 : 1;
            // Insertion , Deletion, Match
            dist[i+j*(n+1)] = std::min({dist[(i-1)+j*(n+1)] + 1,
                                        dist[i+(j-1)*(n+1)] + 1,
                                        dist[(i-1)+(j-1)*(n+1)] + cost
                                       });
        }
    }

    int distance = dist[n+m*(n+1)];
    delete[] dist;
    return distance;
}



inline bool match(const std::pair<double, double> &p1, const std::pair<double, double> &p2, double matchingThreshold)
{

    return std::fabs(p1.first - p2.first) <= matchingThreshold &&
           std::fabs(p1.second - p2.second) <= matchingThreshold;
}
