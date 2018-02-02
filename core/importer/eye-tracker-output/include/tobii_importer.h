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

#ifndef TOBII_IMPORTER_H
#define TOBII_IMPORTER_H


#include <iostream> 
#include <sstream> 
#include <algorithm>
#include <string>
#include <fstream>
#include <exception>
#include <vector>

#include "fast-cpp-csv-parser/csv.h"

/*
 *  - Eye tracker data output based on 'tobii-pro-t60xl-user-manual' 
 *
 *  - Also explained in chapter '5.1. Das Datenformat' : Visuelle Analyse von
 *    Eye-Tracking-Daten by Xuemei Chen. 
 *
 * This structure contains all gaze data we are interested in our analysis.
 * Even if some fields are not needed by the caller, there we will be no great
 * overhead.
 */
struct TobiiExportData {

    // X/Y coordinate for the fixation point mapped to the media coordinate 
    // system, where the origin for the coordinate system is the top 
    // left corner of the image/media
    int mappedFixationPointX;
    int mappedFixationPointY;

    // TODO
    int gazePointX;
    int gazePointY;

    // Indexes for the fixation points (values correspond to the order of the fixation).
    int fixationIndex;
    // Fixation duration. The time in milliseconds that a fixation lasts
    long fixationDuration;
    
    // X/Y coordinate for the raw gaze data point mapped to the media 
    // coordinate system where the origin for the coordinate system 
    // is the top left corner of the image/media
    int mappedGazeDataPointX;
    int mappedGazeDataPointY;

    // Timestamp in milliseconds for when the gaze data was collected
    long timestamp;

    // Validity code for each data point. This code is is based on whether the system is able to identify
    // correctly both eyes. It is recommended that the validity codes should always be used for data filtering,
    // to remove data points that are obviously incorrect. 
    // Values between '0' for good qualitity and '4' for bad qualitity.
    int validity;


    // Media size dimensions for checking Mapped Gaze Data Point validity
    int mediaWidth;
    int mediaHeight;


    int fixationTimestamp;

};


struct Fixation {
    long timestamp;
    int fixationPointX;
    int fixationPointY;
    long fixationDuration;
};


class NumberOfColumnsError: public std::exception
{
  virtual const char* what() const throw()
  {
    return "tobii_importer ::: could not parse the 40 expected columns";
  }
};


void extractFixationSequence(std::vector<Fixation> &fixationSequence, const std::vector<TobiiExportData> &exportData);
double getPercantageOfValidSamples(const std::vector<TobiiExportData> &exportData);

class TobiiImporter {

    public:
        TobiiImporter() {}
        ~TobiiImporter() {}

        /*
         * Extracts gaze-data from a specific Tobii-Export file (Tab-Seperated-Values).
         * The data is stored in the given vector.
         * 
         * Returns True if file could be read successfully, otherwise False.
         */
        bool read(const std::string absExportFilePath, std::vector<TobiiExportData> &exportData, bool filterInvalid=true);


    private:
        void skipNoneRowData(std::istream& in, std::stringstream& out);
        std::istream& safeGetline(std::istream& is, std::string& t);

        void splitByTab(std::vector<std::string> &tokens, std::string line);

        template<typename T> inline bool convertTo(T &t, const std::string &token)
        {
            try {
                t = static_cast<T>(std::stof(token));
            } catch(std::invalid_argument &ia) {
                return false;
            }
            return true;
        }
};

#endif // TOBII_IMPORTER_H
