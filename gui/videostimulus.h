#ifndef VIDEOSTIMULUS_H
#define VIDEOSTIMULUS_H

#include <string>
#include "constants.h"

class VideoStimulus
{
public:
    VideoStimulus();
    bool loadStimulus(const std::string stimulusPath);
private:
    VideoInfo videoInfo;
    std::string stimuliSource;
    bool stimuliSet = false;
};

#endif // VIDEOSTIMULUS_H
