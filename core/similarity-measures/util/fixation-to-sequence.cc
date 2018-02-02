// Copyright: (2017) Maurice Koch
// License: BSD-3
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "fixation-to-sequence.h"


std::string mergeByFixationIndex(const std::string &seq, const std::vector<TobiiExportData> &data)
{
    assert(seq.length() == data.size());
    int n = seq.length();
    std::string mergedSeq = "";

    int lastFixationIndex = -1234;

    for(int i = 0; i < n; i++) {
        if (lastFixationIndex != data[i].fixationIndex) {
            mergedSeq += seq[i];
            lastFixationIndex = data[i].fixationIndex;
        }
    }
    return mergedSeq;
}

std::string fixationSequenceToString(std::vector<Fixation> &fixationSequence,
                                     std::vector<struct DynamicAOI> dynamicAOIs,
                                     long timestampOffset,
                                     double fpms,
                                     bool useTemporalBinning,
                                     unsigned int msBinSize)
{
    if (useTemporalBinning && msBinSize == static_cast<unsigned int>(0)) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: bin size must not be zero");
    }

    // Currently we do not allows more than 26 AOIs
    if (dynamicAOIs.size() > 26) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: currently more than 26 AOIs are not yet supported");
    }

    if (fixationSequence.empty()) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: input data is empty");
    }

    std::string sequence = "";
    long multipleHits = 0L;

    for (const auto &fixation : fixationSequence) {
        long atFrame = (fixation.timestamp - timestampOffset) * fpms;
        char aoiCharLabel = 'a';
        bool withinAOI = false;

        // For each of the definied AOIs.
        // Mind that the multiple AOIs might be active at the same time,
        // so affiliation of fixations to AOIs is not exclusive.
        for (const auto &aoi : dynamicAOIs) {
            // Check if it is within the timespan of the AOI
            if (atFrame >= aoi.framespanStart && atFrame <= aoi.framespanEnd) {

                int elementIndex = findAOIPosition(aoi.elements, atFrame);

                // Error : This should not happen.
                if (elementIndex == -1) {
                    // Throw exception
                    throw FrameElementNotFound();
                }

                // At this point the index is valid
                AOIElement element = aoi.elements[elementIndex];

                // Check if current fixation point is within the AOI
                if (fixation.fixationPointX >= element.x
                        && fixation.fixationPointX < element.x + element.width
                        && fixation.fixationPointY >= element.y
                        && fixation.fixationPointY < element.y + element.height)
                {
                    if (withinAOI) {
                        multipleHits++;
                    }
                    else if (useTemporalBinning) {
                        int repeatTimes = fixation.fixationDuration / msBinSize;
                        if (repeatTimes > 0) {
                            // Repeat character (fixationDuration / msBinSize) times
                            sequence += std::string(repeatTimes, aoiCharLabel);
                        }
                        else {
                            sequence += aoiCharLabel;
                        }
                    } else {
                        sequence += aoiCharLabel;
                    }
                    withinAOI = true;
                }
            }
            // Next AOI -> Next character
            aoiCharLabel++;
        }
        if (!withinAOI) {
            if (useTemporalBinning) {
                int repeatTimes = fixation.fixationDuration / msBinSize;
                if (repeatTimes > 0) {
                    // Repeat character (fixationDuration / msBinSize) times
                    sequence += std::string(repeatTimes, '#');
                }
                else {
                    sequence += '#';
                }
            } else {
                sequence += '#';
            }
        }
    }

    //std::cout << "Double hits = " << multipleHits << std::endl;
    return sequence;
}

std::string gazeSequenceToString(std::vector<TobiiExportData> &exportData, std::vector<DynamicAOI> dynamicAOIs, double fpms)
{
    // Currently we do not allows more than 26 AOIs
    if (dynamicAOIs.size() > 26) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: currently more than 26 AOIs are not yet supported");
    }

    if (exportData.empty()) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: input data is empty");
    }

    long lastTimestamp = exportData.front().timestamp;
    double frameBin = 0.0;
    // Where we store the string represenation this fixation sequence (scanpath)
    std::string sequence = "";
    long atFrame = 1L;
    long multipleHits = 0L;

    for (const auto &data : exportData) {
        frameBin += (data.timestamp - lastTimestamp) * fpms;
        lastTimestamp = data.timestamp;

        if (frameBin >= 1.0) {
            atFrame += static_cast<long>(frameBin);
            frameBin = frameBin - static_cast<long>(frameBin);
        }

        char aoiCharLabel = 'a';
        bool withinAOI = false;

        // For each of the definied AOIs.
        // Mind that the multiple AOIs might be active at the same time,
        // so affiliation of fixations to AOIs is not exclusive.
        for (const auto &aoi : dynamicAOIs) {
            // Check if it is within the timespan of the AOI
            if (atFrame >= aoi.framespanStart && atFrame <= aoi.framespanEnd) {

                int elementIndex = findAOIPosition(aoi.elements, atFrame);

                // Error : This should not happen.
                if (elementIndex == -1) {
                    // Throw exception
                    throw FrameElementNotFound();
                }

                // At this point the index is valid
                AOIElement element = aoi.elements[elementIndex];

                // Check if current fixation point is within the AOI
                if (data.mappedGazeDataPointX >= element.x
                        && data.mappedGazeDataPointX < element.x + element.width
                        && data.mappedGazeDataPointY >= element.y
                        && data.mappedGazeDataPointY < element.y + element.height)
                {
                    if (withinAOI) {
                        multipleHits++;
                    }
                    else {
                        sequence += aoiCharLabel;
                    }
                    withinAOI = true;
                }
            }
            // Next AOI -> Next character
            aoiCharLabel++;
        }
        if (!withinAOI) {
            sequence += '#';
        }
    }

    //std::cout << "Double hits = " << multipleHits << std::endl;
    return sequence;
}


std::string fixationToSequenceDynamic(std::vector<struct TobiiExportData> &exportData,
                                      std::vector<struct DynamicAOI> dynamicAOIs,
                                      double fpms,
                                      bool useTemporalBinning,
                                      unsigned int msBinSize)
{
    if (useTemporalBinning && msBinSize == static_cast<unsigned int>(0)) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: bin size must not be zero");
    } 

    // Currently we do not allows more than 26 AOIs
    if (dynamicAOIs.size() > 26) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: currently more than 26 AOIs are not yet supported");
    }

    if (exportData.empty()) {
        throw std::invalid_argument("fixationToSequenceDynamic ::: input data is empty");
    }

    long lastTimestamp = exportData.front().timestamp;
    double frameBin = 0.0;
    // Where we store the string represenation this fixation sequence (scanpath)
    std::string sequence = "";
    long atFrame = 1L;
    long multipleHits = 0L;

    for (const auto &data : exportData) {
        frameBin += (data.timestamp - lastTimestamp) * fpms;
        lastTimestamp = data.timestamp;

        if (frameBin >= 1.0) {
            atFrame += static_cast<long>(frameBin);
            frameBin = frameBin - static_cast<long>(frameBin);
        }

        char aoiCharLabel = 'a';
        bool withinAOI = false;

        // For each of the definied AOIs.
        // Mind that the multiple AOIs might be active at the same time,
        // so affiliation of fixations to AOIs is not exclusive.
        for (const auto &aoi : dynamicAOIs) {
            // Check if it is within the timespan of the AOI
            if (atFrame >= aoi.framespanStart && atFrame <= aoi.framespanEnd) {

                int elementIndex = findAOIPosition(aoi.elements, atFrame);

                // Error : This should not happen.
                if (elementIndex == -1) {
                    // Throw exception
                    throw FrameElementNotFound();
                }

                // At this point the index is valid
                AOIElement element = aoi.elements[elementIndex];

                // Check if current fixation point is within the AOI
                if (data.mappedFixationPointX >= element.x
                        && data.mappedFixationPointX < element.x + element.width
                        && data.mappedFixationPointY >= element.y
                        && data.mappedFixationPointY < element.y + element.height)
                {
                    if (withinAOI) {
                        multipleHits++;
                    }
                    else if (useTemporalBinning) {
                        int repeatTimes = data.fixationDuration / msBinSize;
                        if (repeatTimes > 0) {
                            // Repeat character (fixationDuration / msBinSize) times
                            sequence += std::string(repeatTimes, aoiCharLabel);
                        }
                        else {
                            sequence += aoiCharLabel;
                        }
                    } else {
                        sequence += aoiCharLabel;
                    }
                    withinAOI = true;
                }
            }
            // Next AOI -> Next character
            aoiCharLabel++;
        }
        if (!withinAOI) {
            if (useTemporalBinning) {
                int repeatTimes = data.fixationDuration / msBinSize;
                if (repeatTimes > 0) {
                    // Repeat character (fixationDuration / msBinSize) times
                    sequence += std::string(repeatTimes, '#');
                }
                else {
                    sequence += '#';
                }
            } else {
                sequence += '#';
            }
        }
    }

    //std::cout << "Double hits = " << multipleHits << std::endl;
    return sequence;
}

//TODO : MAKE THIS SAFE
int findAOIPosition(const std::vector<struct AOIElement> &elements, long frame)
{
    // Do Binary Search
    int l = 0;
    int r = elements.size() - 1;

    if (l == r) {
        return l;
    }
 
    while (l <= r) {
        int m = l + (r - l) / 2;
        if(frame < elements[m].framespanStart) {
            r = m - 1;
        }
        else if (frame > elements[m].framespanEnd) {
            l = m + 1;
        }
        else {
            return m;
        }
    }
    // Frame is not within the timespan of the AOI
    return -1;
}
