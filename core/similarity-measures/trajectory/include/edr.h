#ifndef EDR_H
#define EDR_H

#include <cmath>
#include <vector>
#include <stdexcept>
#include "util.h"
#include "../../../importer/eye-tracker-output/include/tobii_importer.h"


double scoreEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                bool normalize, double matchingThreshold = 100.);

int distanceEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                double matchingThreshold);
int distanceEDR(const struct Trajectory& trP1,
                const struct Trajectory& trP2,
                double matchingThreshold,
                bool normalize);
inline bool match(const std::pair<double, double> &p1, const std::pair<double, double> &p2, double matchingThreshold);
#endif // EDR_H
