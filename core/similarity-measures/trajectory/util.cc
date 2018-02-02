#include "util.h"

/*
struct Trajectory createTrajectory(const std::vector<Fixation> &in)
{
    struct Trajectory trajectory;
    for(const auto& data : in) {
        std::pair<double, double> point = {data.fixationPointX, data.fixationPointY};
        trajectory.points.push_back(point);
    }
    return trajectory;
}
*/
struct Trajectory createTrajectoryFromGazeData(const std::vector<struct TobiiExportData> &in)
{
    struct Trajectory trajectory;
    for(const auto& data : in) {
        std::pair<double, double> point = {data.mappedFixationPointX, data.mappedFixationPointY};
        trajectory.points.push_back(point);
    }
    return trajectory;
}
struct Trajectory createTrajectoryFromFixationData(const std::vector<struct TobiiExportData> &in)
{
    struct Trajectory trajectory;
    for(const auto& data : in) {
        std::pair<double, double> point = {data.mappedGazeDataPointX, data.mappedGazeDataPointX};
        trajectory.points.push_back(point);
    }
    return trajectory;
}


// This is a z-normalisation (precisely incorrect : studentiation),
// such that mean is afterwards zero and the standard deviation is afterwards one.
struct NormalizedTrajectory normalizeTrajectory(const struct Trajectory& in)
{
    if(in.points.empty()) {
       throw std::invalid_argument("normalizeTrajectory ::: sequence is empty");
    }

    struct NormalizedTrajectory normalized;

    normalized.meanX = 0.;
    normalized.meanY = 0.;

    // Calculate (sampling) mean values
    for(const auto& data : in.points) {
        normalized.meanX += data.first;
        normalized.meanY += data.second;
    }

    normalized.meanX /= in.points.size();
    normalized.meanY /= in.points.size();

    // Calculate (sampling) variance values
    double varX = 0.;
    double varY = 0.;

    for(const auto& data : in.points) {
        varX += std::pow(data.first - normalized.meanX, 2);
        varY += std::pow(data.second - normalized.meanY, 2);
    }
    varX /= in.points.size();
    varY /= in.points.size();

    // Calculate (sampling) standard deviation values
    normalized.stdX = std::sqrt(varX);
    normalized.stdY = std::sqrt(varY);

    // Now we can normalize by substracting the mean
    // and dividing by the standard deviation
    for(auto& data : in.points) {
        std::pair<double, double> point(zNormalize(data.first, normalized.meanX, normalized.stdX),
                                        zNormalize(data.second, normalized.meanY, normalized.stdY));

        normalized.points.push_back(point);
    }

    return normalized;
}
