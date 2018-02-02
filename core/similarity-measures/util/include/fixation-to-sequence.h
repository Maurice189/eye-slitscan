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

#ifndef FIXATION_TO_SEQUENCE_H
#define FIXATION_TO_SEQUENCE_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

#include "../../../importer/eye-tracker-output/include/tobii_importer.h"
#include "../../../importer/aoi/include/aoi-importer.h"


const std::string FIXATION_ERROR_CODE = "--ERROR-SCANPATH--";


class FrameElementNotFound: public std::exception
{
  virtual const char* what() const throw()
  {
    return "fixationToSequenceDynamic ::: could not find frame element in aoi definition";
  }
};
std::string mergeByFixationIndex(const std::string &seq, const std::vector<TobiiExportData> &data);

/*
 * 
 * Generates the correct fixation string sequence from given Tobii gaze data
 * and dynamic stimuli. Dynamic stimuli implies that the attributes of AOIs
 * change dynamically, too. 
 *
 */
std::string fixationToSequenceDynamic(std::vector<struct TobiiExportData> &exportData,
                                      std::vector<struct DynamicAOI> dynamicAOIS,
                                      double fpms,
                                      bool useTemporalBinning = false,
                                      unsigned int msBinSize = 50);

std::string gazeSequenceToString(std::vector<struct TobiiExportData> &exportData,
                                 std::vector<struct DynamicAOI> dynamicAOIS,
                                 double fpms);

std::string fixationSequenceToString(std::vector<Fixation> &fixationSequence,
                                     std::vector<struct DynamicAOI> dynamicAOIs,
                                     long timestampOffset,
                                     double fpms,
                                     bool useTemporalBinning = false,
                                     unsigned int msBinSize=100);

/*
 * The given 'frame' is searched by Binary Search within the AOI elements.
 * If the correct AOI element has been found such that 'frame' is within its
 * frame window, the corresponding index is returned. If 'frame' is in neither
 * frame window, the error index '-1' is returned.
 */
int findAOIPosition(const std::vector<struct AOIElement> &elements, long frame);

#endif
