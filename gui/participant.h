#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "constants.h"

class Participant
{
public:

    Participant(std::vector<struct DynamicAOI> &dynamicAOIs, VideoInfo info, std::string stimuliSource);
    Participant();
    void load(const std::string &absoluteFilePath, const std::string &label, int n, int slitScanHeight);
private:
    std::vector<struct DynamicAOI> dynamicAOIs;
    VideoInfo info;
    std::string stimuliSource;
public:
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

#endif // PARTICIPANT_H
