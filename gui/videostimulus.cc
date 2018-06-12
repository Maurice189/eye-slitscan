#include "videostimulus.h"

VideoStimulus::VideoStimulus()
{

}

bool VideoStimulus::loadStimulus(const std::string stimulusPath)
{
    try {
        this->videoInfo = SlitscanGenerator::readVideoInfo(stimulusPath);
        this->stimuliSource = stimulusPath;
        this->stimuliSet = true;

        return true;
    } catch (SlitScanVideoError &error) {
    }
    return false;
}
