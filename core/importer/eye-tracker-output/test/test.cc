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

#include <gtest/gtest.h>
#include "tobii_importer.h"

TEST(VALIDITY_CHECK, ROW_DATA) {

    TobiiImporter importer;
    const std::string TOBII_EXPORT_FILE_1 = "Tobii_exports/01-OK.tsv";
    const std::string TOBII_EXPORT_FILE_2 = "Tobii_exports/02-OK.tsv";
    std::vector<TobiiExportData> exportData;
    std::vector<Fixation> fixationSequence;
    
    // Test with first file
    ASSERT_TRUE(importer.read(TOBII_EXPORT_FILE_1, exportData)) << "Error reading export file";
    extractFixationSequence(fixationSequence, exportData);

    EXPECT_EQ(exportData[0].timestamp, 911097) << "Error : Wrong timestamp.";
    EXPECT_EQ(exportData[1].timestamp, 911098) << "Error : Wrong timestamp.";
    EXPECT_EQ(exportData[1].fixationDuration, 766) << "Error : Wrong fixation duration.";
    EXPECT_EQ(exportData[1].fixationIndex, 2349) << "Error : Wrong fixation index.";
    EXPECT_EQ(exportData[1].validity, 0) << "Error : Wrong validity";
    EXPECT_EQ(exportData[1].mappedFixationPointX, 808) << "Error : Wrong mapped fixation point X.";
    EXPECT_EQ(exportData[1].mappedFixationPointY, 591) << "Error : Wrong mapped fixation point X.";
    EXPECT_EQ(exportData[1].mappedGazeDataPointX, 813) << "Error : Wrong mapped gaza data point X.";
    EXPECT_EQ(exportData[1].mappedGazeDataPointY, 595) << "Error : Wrong mapped gaze data point Y.";
    EXPECT_EQ(exportData.back().timestamp, 930160) << "Error : Wrong timestamp.";


    EXPECT_EQ(fixationSequence[1].fixationPointX, 808);
    EXPECT_EQ(fixationSequence[1].fixationPointY, 591);
    EXPECT_EQ(fixationSequence[1].fixationDuration, 766);

    EXPECT_EQ(fixationSequence[2].fixationPointX, 536);
    EXPECT_EQ(fixationSequence[2].fixationPointY, 509);
    EXPECT_EQ(fixationSequence[2].fixationDuration, 233);

    EXPECT_EQ(fixationSequence[3].fixationPointX, 578);
    EXPECT_EQ(fixationSequence[3].fixationPointY, 552);
    EXPECT_EQ(fixationSequence[3].fixationDuration, 1749);

    EXPECT_EQ(fixationSequence[4].fixationPointX, 1465);
    EXPECT_EQ(fixationSequence[4].fixationPointY, 461);
    EXPECT_EQ(fixationSequence[4].fixationDuration, 150);

    EXPECT_EQ(fixationSequence[fixationSequence.size()-2].fixationPointX, 1062);
    EXPECT_EQ(fixationSequence[fixationSequence.size()-2].fixationPointY, 490);
    EXPECT_EQ(fixationSequence[fixationSequence.size()-2].fixationDuration, 450);

    exportData.clear();
    fixationSequence.clear();

    // Test with second file 
    ASSERT_TRUE(importer.read(TOBII_EXPORT_FILE_2, exportData)) << "Error reading export file";
    EXPECT_EQ(exportData[0].timestamp, 16102) << "Error : Wrong timestamp.";
    EXPECT_EQ(exportData[1].timestamp, 16115) << "Error : Wrong timestamp.";
    EXPECT_EQ(exportData[1].fixationDuration, 466) << "Error : Wrong fixation duration.";
    EXPECT_EQ(exportData[1].fixationIndex, 47) << "Error : Wrong fixation index.";
    EXPECT_EQ(exportData[1].validity, 0) << "Error : Wrong validity";
    EXPECT_EQ(exportData[1].mappedFixationPointX, 885) << "Error : Wrong mapped fixation point X.";
    EXPECT_EQ(exportData[1].mappedFixationPointY, 498) << "Error : Wrong mapped fixation point X.";
    EXPECT_EQ(exportData[1].mappedGazeDataPointX, 880) << "Error : Wrong mapped gaza data point X.";
    EXPECT_EQ(exportData[1].mappedGazeDataPointY, 486) << "Error : Wrong mapped gaze data point Y.";
    EXPECT_EQ(exportData.back().timestamp, 35171) << "Error : Wrong timestamp.";
}

TEST(VALIDITY_CHECK, FILE_FORMAT) {

    TobiiImporter importer;
    const std::string TOBII_EXPORT_FILE_OK1 = "Tobii_exports/01-OK.tsv";
    const std::string TOBII_EXPORT_FILE_OK2 = "Tobii_exports/02-OK.tsv";
    const std::string TOBII_EXPORT_FILE_BAD1 = "Tobii_exports/01-BAD.tsv";
    const std::string TOBII_EXPORT_FILE_BAD2 = "Tobii_exports/02-BAD.tsv";

    std::vector<TobiiExportData> exportData;
    
    EXPECT_TRUE(importer.read(TOBII_EXPORT_FILE_OK1, exportData)) << "Error reading export file";
    EXPECT_TRUE(importer.read(TOBII_EXPORT_FILE_OK2, exportData)) << "Error reading export file";
    EXPECT_FALSE(importer.read("Tobii_exports/no.tsv", exportData)) << "Error reading export file";
    //TODO : Handle this kind of malformed input !
    //EXPECT_FALSE(importer.read(TOBII_EXPORT_FILE_BAD1, exportData)) << "Error reading export file";
    //EXPECT_FALSE(importer.read(TOBII_EXPORT_FILE_BAD2, exportData)) << "Error reading export file";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

