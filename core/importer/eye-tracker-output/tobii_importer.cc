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


#include "tobii_importer.h"


void extractFixationSequence(std::vector<Fixation> &fixationSequence, const std::vector<TobiiExportData> &exportData)
{
    int lastFixationIndex = -12345;

    for(const auto &data : exportData) {
        if (data.fixationIndex != lastFixationIndex) {
            Fixation fixation = {data.timestamp, data.mappedFixationPointX, data.mappedFixationPointY, data.fixationDuration};
            fixationSequence.push_back(fixation);
            lastFixationIndex = data.fixationIndex;
        }
    }
}

double getPercantageOfValidSamples(const std::vector<TobiiExportData> &exportData)
{
    int nofInvalid = 0;
    for(const auto &data : exportData) {
        nofInvalid += data.validity >= 4 ? 1 : 0;
    }
    return 1. - (double(nofInvalid) / exportData.size());
}

/* Reads the TSV formatted Tobii output data from a file. Gaze data is stored in the passed list. */
bool TobiiImporter::read(const std::string tsvPath, std::vector<struct TobiiExportData> &exportData, bool filterInvalid)
{
    std::ifstream infile(tsvPath);

    if(!infile.is_open()) {
        return false;
    }

    std::string line;

    int indexTimeStamp = 0;
    int indexValidLeft = -1;
    int indexValidRight = -1;
    int indexFixationIndex = -1;
    int indexFixationDuration = -1;
    int indexGazePointX = -1;
    int indexGazePointY = -1;
    int indexMediaWidth = -1;
    int indexMediaHeight = -1;
    int indexMGazePointX = -1;
    int indexMGazePointY = -1;
    int indexMFixationX = -1;
    int indexMFixationY = -1;

    for (int l = 0; safeGetline(infile, line); ++l) {
        std::vector<std::string> tokens;
        splitByTab(tokens, line);

        if (tokens.size() > 0 && tokens[0] == "Timestamp") {

            for(int i = 1; i < tokens.size(); i++) {
                if (tokens[i] == "GazePointX") indexGazePointX = i;
                else if (tokens[i] == "GazePointY") indexGazePointY = i;
                else if (tokens[i] == "MappedGazeDataPointX") indexMGazePointX = i;
                else if (tokens[i] == "MappedGazeDataPointY") indexMGazePointY = i;
                else if (tokens[i] == "MappedFixationPointX") indexMFixationX = i;
                else if (tokens[i] == "MappedFixationPointY") indexMFixationY = i;
                else if (tokens[i] == "ValidityLeft") indexValidLeft = i;
                else if (tokens[i] == "ValidityRight") indexValidRight = i;
                else if (tokens[i] == "MediaWidth") indexMediaWidth = i;
                else if (tokens[i] == "MediaHeight") indexMediaHeight = i;
                else if (tokens[i] == "FixationIndex") indexFixationIndex = i;
                else if (tokens[i] == "FixationDuration") indexFixationDuration = i;
            }

            break;
        }
    }

    auto bounds = std::minmax({indexGazePointX, indexGazePointY, indexMGazePointX, indexMGazePointY,
                              indexMFixationX, indexMFixationY, indexValidLeft, indexValidRight,
                              indexMediaWidth, indexMediaHeight, indexFixationIndex, indexFixationDuration});

    if (bounds.first == -1) {
        return false;
    }


    int lastFixationIndex = -1;
    int fixationTimestamp;


    // Read TSV line by line
    for (int l = 0; safeGetline(infile, line); ++l) {
        struct TobiiExportData rowData = {0,0,0,0,0,0,0,0,0,0};
        // Used to check if gaze data is valid
        int valid = 0;
        std::vector<std::string> tokens;
        splitByTab(tokens, line);

        if (tokens.size() <= bounds.second) {
            continue;
        }


        try {
            int validLeft = 0;
            int validRight = 0;

            convertTo(rowData.timestamp, tokens[indexTimeStamp]);
            convertTo(validLeft, tokens[indexValidLeft]);
            convertTo(validRight, tokens[indexValidRight]);
            convertTo(rowData.fixationIndex, tokens[indexFixationIndex]);
            convertTo(rowData.gazePointX, tokens[indexGazePointX]);
            convertTo(rowData.gazePointY, tokens[indexGazePointY]);
            convertTo(rowData.mediaWidth, tokens[indexMediaWidth]);
            convertTo(rowData.mediaHeight, tokens[indexMediaHeight]);
            convertTo(rowData.mappedFixationPointX, tokens[indexMFixationX]);
            convertTo(rowData.mappedFixationPointY, tokens[indexMFixationY]);
            convertTo(rowData.fixationDuration, tokens[indexFixationDuration]);
            convertTo(rowData.mappedGazeDataPointX, tokens[indexMGazePointX]);
            convertTo(rowData.mappedGazeDataPointY, tokens[indexMGazePointY]);

            if (rowData.fixationIndex != lastFixationIndex) {
                fixationTimestamp = rowData.timestamp;
                lastFixationIndex = rowData.fixationIndex;
            }

            rowData.fixationTimestamp = fixationTimestamp;

            valid += validLeft;
            valid += validRight;
        // Most likely when std::stoi throws an exception
        } catch(const std::invalid_argument& ia) {
            std::cerr << "Invalid argument for integer conversion at line " << l << std::endl;
        }

        rowData.validity = valid;

        if (!filterInvalid || valid == 0) {
            exportData.push_back(rowData);
        }
    }
    return true;
}

void TobiiImporter::splitByTab(std::vector<std::string> &tokens, std::string line)
{
    std::istringstream iss(line);
    std::string token;
    while(std::getline(iss, token, '\t'))   // but we can specify a different one
        tokens.push_back(token);
}


// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
// TODO : CHECK THE CORRECTNESS OF THIS PROCEDURE - DOES IT TERMINATE GUARANTALLY ?
std::istream& TobiiImporter::safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if(sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if(t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += static_cast<char>(c);
        }
    }
}
