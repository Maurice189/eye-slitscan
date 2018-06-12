#ifndef SCANPATH_CLUSTERING_H
#define SCANPATH_CLUSTERING_H

#include "constants.h"
#include "participant.h"

class ScanpathClustering
{
public:
    ScanpathClustering();
    ClusteringStateData execute(std::vector<Participant> &checkedParticipant, MeasureInformation measure, ClusteringMethod method, QPair<int, int> xRange);
    SimilarityMatrix createSimilarityMatrix(MeasureInformation measure, QPair<int, int> xRange, std::vector<Participant> &checkedParticipants);
    double calculateSimilarityScore(const TimeSpanData &p1Data, const TimeSpanData &p2Data, MeasureInformation measure);
};

#endif // SCANPATH_CLUSTERING_H
