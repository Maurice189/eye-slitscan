#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "../core/similarity-measures/trajectory/include/util.h"
#include "../core/slitscan-generator/include/genslitscan.h"
#include "../core/importer/aoi/include/aoi-importer.h"
#include "../core/similarity-measures/util/include/fixation-to-sequence.h"
#include "../core/similarity-measures/scanpath/include/string_edit.h"
#include "../core/similarity-measures/trajectory/include/dtw.h"
#include "../core/similarity-measures/trajectory/include/edr.h"
#include "../core/similarity-measures/trajectory/include/frechet.h"
#include "../core/similarity-measures/trajectory/include/euclidean.h"
#include "../core/similarity-measures/image/include/histo-measure.h"
#include "../core/similarity-measures/image/include/image-measure.h"
#include "../core/importer/eye-tracker-output/include/tobii_importer.h"
#include "../core/clustering/include/hac.h"
#include "../core/clustering/helper.h"

#include <QMap>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QList>
#include <QColor>

enum class SimilarityMeasure {LEVENSHTEIN, NEEDLEMAN_WUNSCH, DTW, EUCLIDEAN, FRECHET, HISTOGRAM, IMAGE2D, MM_VEC, MM_POS, MM_DUR, MM_ANGLE, MM_LEN, MULTIMATCH};
enum class SimilarityClass {SCANPATH, TRAJECTORY, IMAGE};

const std::vector<std::function<int(char, char)>> SUBSTITUTION_MATRICES = {
    // 01-car pursuit
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 02-turning car
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 03-dialog

    // a - Person 1 , b - Person 2, c - T-Shirt-Labe of Person 2
    [](char a, char b) {
        if(a == b) {
            return 4;
        }
        // AOIs of Persons should be more similar, than people looking at the T-Shirt-Label
        if ((a == 'a' && b == 'b') || (a == 'b' && b == 'a')) {
            return 1;
        }
        return -1;
    },
    // 04-thimblerig
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 05-memory
    // Each of the 16 cards is an AOI.

    // a b c d
    // e f g h
    // i j k l
    // m n o p
    [](char a, char b) {
        if(a == b)
            return 7;

        // If exactly one is looking not at any card
        // then we penalize this.
        if (a == '#' || b == '#')
            return -1;

        // AOIs that are spatially nearer to each other
        // are more similary to each other.
        int indexA = a - 'a';
        int indexB = b - 'a';

        int xA = indexA % 4;
        int yA = (indexA - xA) / 4;

        int xB = indexB % 4;
        int yB = (indexB - xB) / 4;

        // [0,1] -> From near to far (spatially).
        double distance = std::sqrt(std::pow(xA-xB, 2) + std::pow(yA-yB, 2)) / std::sqrt(18);
        // [0,1] -> From far to near (spatially).
        double score = 1. - distance;
        // [0,4] -> From far to near
        return int(score * 4);
    },
    // 06-UNO

    // Left hand : A
    // Right hand : B
    // Tray Deck : C
    // Deck : D
    [](char a, char b) {
        if(a == b)
            return 4;
        // AOIs of hands are more similar
        if ((a == 'a' && b == 'b') || (a == 'b' && b == 'a')) {
            return 1;
        }
        return -1;
    },
    // 07-kite
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 08-case exchange
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 09-ball game
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 010-bag search
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
    // 011-person search
    [](char a, char b) {
        if(a == b)
            return 1;
        return -1;
    },
};


const QList<QColor> COLORS = {
    Qt::red,
    Qt::blue,
    Qt::green,
    Qt::yellow,
    Qt::cyan,
    Qt::magenta,
    Qt::gray,
    Qt::darkRed,
    Qt::darkBlue,
    Qt::darkGreen,
    Qt::darkYellow,
    Qt::darkCyan,
    Qt::darkMagenta,
    Qt::darkGray,
    Qt::white,
    Qt::black
};

struct MeasureInformation {
    QString title;
    SimilarityMeasure measure;
    QMap<QString, QString> properties;
    bool invertValues;
};

struct ClusterInfo {
    int from;
    int to;
    MeasureInformation info;
};

struct ParticipantData {
    QString id;
    QColor color;
    int insertIndex;

    bool active = true;

    std::vector<TobiiExportData> exportData;
    std::string aoiStringWTB;
    cv::Mat matSlitscan;
    cv::Mat matVC123;
    QImage imgSpectrogram;
    QPixmap imgSlitscan;

};

struct TimeSpanData {
    Trajectory trajectoryOfGazes;
    Trajectory trajectoryOfFixations;

    std::string aoiString;
    std::string aoiStringWTB;

    cv::Mat slitscan;
};

struct SimilarityMatrix {
    QString name;
    MeasureInformation info;
    QList<QString> labels;
    std::vector<std::vector<double>> elements;
    bool highlightEntries=false;
};

struct ClusteringStateData {
    SimilarityMatrix matrix;
    ClusteringResult result;
    ClusterInfo info;
};


#endif // CONSTANTS_H
