#include "scanpath_clustering.h"

ScanpathClustering::ScanpathClustering()
{

}

ClusteringStateData ScanpathClustering::execute(std::vector<Participant> &checkedParticipant, MeasureInformation measure, ClusteringMethod method, QPair<int, int> xRange)
{
    ClusterInfo info;
    info.info = measure;
    info.from = xRange.first;
    info.to = xRange.second;

    auto similarityMatrix = createSimilarityMatrix(measure, xRange, checkedParticipant);

    clu::CSimilarityMatrix matrix;
    matrix.elements = similarityMatrix.elements;

    for (auto label : similarityMatrix.labels) {
        matrix.labels.push_back(label.toStdString());
    }

    Clustering clustering(matrix, method);
    ClusteringResult result = clustering.executeHAC();

    ClusteringStateData clusteringState;

    clusteringState.matrix = createSimilarityMatrix(measure, xRange, checkedParticipant);
    clusteringState.matrix.name = measure.title;
    clusteringState.info = info;
    clusteringState.result = result;

    return clusteringState;

}

SimilarityMatrix ScanpathClustering::createSimilarityMatrix(MeasureInformation measure, QPair<int, int> xRange, std::vector<Participant> &checkedParticipants)
{
    SimilarityMatrix similarityMatrix;
    std::vector<TimeSpanData> spansFromParticipants;

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        Participant &data = checkedParticipants[i];

        auto begin = data.exportData.begin() + xRange.first;
        auto end = data.exportData.begin() + xRange.second;
        auto exportData = std::vector<TobiiExportData>(begin, end);

        TimeSpanData timeSpanData;
        timeSpanData.aoiStringWTB = data.aoiStringWTB.substr(xRange.first, xRange.second - xRange.first);
        timeSpanData.aoiString = mergeByFixationIndex(timeSpanData.aoiStringWTB, exportData);
        timeSpanData.trajectoryOfGazes = createTrajectoryFromGazeData(exportData);
        timeSpanData.trajectoryOfFixations = createTrajectoryFromFixationData(exportData);
        timeSpanData.slitscan = data.matSlitscan(cv::Rect(xRange.first, 0, xRange.second-xRange.first, data.matSlitscan.rows)).clone();

        spansFromParticipants.push_back(timeSpanData);

        std::cout << data.id.toStdString() << " binning : " << timeSpanData.aoiStringWTB << std::endl;
        std::cout << data.id.toStdString() << " merged : " << timeSpanData.aoiString << std::endl;
    }

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        std::vector<double> column;
        column.resize(checkedParticipants.size());
        similarityMatrix.elements.push_back(column);
    }

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        similarityMatrix.labels.append(checkedParticipants[i].id);
        for(int j = i+1; j < checkedParticipants.size(); ++j) {
            double score = calculateSimilarityScore(spansFromParticipants[i], spansFromParticipants[j], measure);
            similarityMatrix.elements[i][j] = score;
            similarityMatrix.elements[j][i] = score;
        }
    }

    createPositiveMatrix(similarityMatrix.elements);

    // Invert values, if necessary
    if (measure.invertValues) {
        invertMatrix(similarityMatrix.elements);
    }

    normalizeMatrix(similarityMatrix.elements);

    similarityMatrix.info = measure;
    return similarityMatrix;
}

double ScanpathClustering::calculateSimilarityScore(const TimeSpanData &p1Data, const TimeSpanData &p2Data, MeasureInformation measure)
{
    switch (measure.measure) {
    case SimilarityMeasure::DTW:
    {
        assert(measure.properties.find("REPRESENTATION") != measure.properties.end());
        auto propRepresentation = measure.properties["REPRESENTATION"];

        if (propRepresentation == "FIXATIONS") {
            return distanceDTW(p1Data.trajectoryOfFixations, p2Data.trajectoryOfFixations, false);
        } else if (propRepresentation == "GAZE-POINTS") {
            return distanceDTW(p1Data.trajectoryOfGazes, p2Data.trajectoryOfGazes, false);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::FRECHET:
    {
        assert(measure.properties.find("REPRESENTATION") != measure.properties.end());
        auto propRepresentation = measure.properties["REPRESENTATION"];

        if (propRepresentation == "FIXATIONS") {
            return discreteFrechetDistance(p1Data.trajectoryOfFixations, p2Data.trajectoryOfFixations);
        } else if (propRepresentation == "GAZE-POINTS") {
            return discreteFrechetDistance(p1Data.trajectoryOfGazes, p2Data.trajectoryOfGazes);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::LEVENSHTEIN:
    {
        assert(measure.properties.find("TEMPORAL-BINNING") != measure.properties.end());
        auto propTemporalBinning = measure.properties["TEMPORAL-BINNING"];

        if (propTemporalBinning == "YES") {
            return levenshteinDistance(p1Data.aoiStringWTB, p2Data.aoiStringWTB);
        } else if (propTemporalBinning == "NO"){
            return levenshteinDistance(p1Data.aoiString, p2Data.aoiString);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::NEEDLEMAN_WUNSCH:
    {
        assert(measure.properties.find("TEMPORAL-BINNING") != measure.properties.end());
        auto propTemporalBinning = measure.properties["TEMPORAL-BINNING"];

        assert(measure.properties.find("SCORING") != measure.properties.end());
        auto propSubstMt = measure.properties["SCORING"];

        std::function<int(char, char)> subWeights;

        if (propSubstMt.left(8) == "STANDARD") {
            std::cout << "Use Standard Substituiton Matrix" << std::endl;
            subWeights = [](char a, char b) {
                if(a == b)
                    return 1;
                return -1;
            };
        } else if (propSubstMt.left(9) == "BENCHMARK") {
            int benchmarkID = propSubstMt.right(2).toInt();
            assert(benchmarkID >= 1 && benchmarkID <= 11);
            std::cout << "Use Substituiton Matrix of benchmark data : " << benchmarkID << std::endl;
            subWeights = SUBSTITUTION_MATRICES[benchmarkID-1];
        } else {
            assert(false);
        }

        if (propTemporalBinning == "YES") {
            return needlemanWunschScore(p1Data.aoiStringWTB, p2Data.aoiStringWTB, subWeights, -1);
        } else if (propTemporalBinning == "NO"){
            return needlemanWunschScore(p1Data.aoiString, p2Data.aoiString, subWeights, -1);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::HISTOGRAM:
    {
        assert(measure.properties.find("COLOR-SPACE") != measure.properties.end());
        assert(measure.properties.find("METRIC") != measure.properties.end());

        auto propMetric = measure.properties["METRIC"];
        auto propColorSpace = measure.properties["COLOR-SPACE"];

        HistogramMetric metric;
        ColorSpace colorSpace;

        if (propMetric == "BHATTACHARYYA") {
            metric = HistogramMetric::BHATTACHARYYA;
        } else if (propMetric == "INTERSECTION") {
            metric = HistogramMetric::INTERSECTION;
        } else if (propMetric == "CORRELATION") {
            metric = HistogramMetric::CORRELATION;
        } else if (propMetric == "CHI_SQUARE") {
            metric = HistogramMetric::CHI_SQUARE;
        } else if (propMetric == "EMD") {
            metric = HistogramMetric::EMD;
        } else {
            assert(false);
        }

        if (propColorSpace == "RGB") {
            colorSpace = ColorSpace::RGB;
        } else if (propColorSpace == "HSV") {
            colorSpace = ColorSpace::HSV;
        } else if (propColorSpace == "LAB") {
            colorSpace = ColorSpace::CIE_LAB;
        } else {
            assert(false);
        }

        return histogramScore(p1Data.slitscan, p2Data.slitscan, metric, colorSpace);
    }
        break;
    case SimilarityMeasure::IMAGE2D:
    {
        assert(measure.properties.find("METRIC") != measure.properties.end());
        auto propMetric = measure.properties["METRIC"];

        ImageMetric metric;

        if (propMetric == "SQUARED_DIFFERENCES") {
            metric = ImageMetric::SQUARED_DIFFERENCES;
        } else if (propMetric == "COSINE_COEFFICIENT") {
            metric = ImageMetric::COSINE_COEFFICIENT;
        } else if (propMetric == "CROSS_CORRELATION") {
            metric = ImageMetric::CROSS_CORRELATION;
        } else {
            assert(false);
        }
        return imageScore(p1Data.slitscan, p2Data.slitscan, metric);
    }
        break;
    default:
        assert(false);
        break;
    }
}

