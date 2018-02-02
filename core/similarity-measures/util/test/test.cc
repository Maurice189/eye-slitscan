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

#include <string>

#include <gtest/gtest.h>
#include <importer/eye-tracker-output/include/tobii_importer.h>
#include <importer/aoi/include/aoi-importer.h>
#include "fixation-to-sequence.h"


TEST(VALIDITY_CHECK, REAL_TEST) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;
    double fpms = 3. / (160 - 100);

    /**
     *  Fixation at time=110 [Frame 1]: Is in active AOI -> a 
     *  Fixation at time=120 [Frame 1]: Is not in active AOI. This AOI becomes active at frame 2 ! -> # 
     *  Fixation at time=130 [Frame 2]: Is in active AOI -> b
     *  Fixation at time=140 [Frame 2]: Is in active AOI -> b
     *  Fixation at time=150 [Frame 3]: Is in active AOI -> c
     *  Fixation at time=160 [Frame 3]: Is not in active AOI -> #
    **/
    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/P9B-01-car pursuit.tsv", exportData)) << "Error reading export file";

    exportData.erase(exportData.begin());
    std::string stringSequence01 = fixationToSequenceDynamic(exportData, dynamicAOIs, fpms);

    //EXPECT_EQ(stringSequence01, "a#bbc#") << "Error : Wrong sequence string for sequence 01.";
}

TEST(VALIDITY_CHECK, BASIC_FUNCTIONALITY) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;

    /**
     *  Fixation at time=110 [Frame 1]: Is in active AOI -> a 
     *  Fixation at time=120 [Frame 1]: Is not in active AOI. This AOI becomes active at frame 2 ! -> # 
     *  Fixation at time=130 [Frame 2]: Is in active AOI -> b
     *  Fixation at time=140 [Frame 2]: Is in active AOI -> b
     *  Fixation at time=150 [Frame 3]: Is in active AOI -> c
     *  Fixation at time=160 [Frame 3]: Is not in active AOI -> #
    **/

    double fpms = 3. / (160 - 100);

    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/01.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/01.tsv", exportData)) << "Error reading export file";

    exportData.erase(exportData.begin());
    std::string stringSequence01 = fixationToSequenceDynamic(exportData, dynamicAOIs, fpms);

    EXPECT_EQ(stringSequence01, "a#bbc#") << "Error : Wrong sequence string for sequence 01.";
}

TEST(VALIDITY_CHECK, MULTIPLE_AOI_HITS_PER_FRAME) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;
    double fpms = 3. / (160 - 100);

    /**
     *  Fixation at time=110 [Frame 1]: Is in active AOI -> a 
     *  Fixation at time=115 [Frame 1]: Is in active AOI -> b 
     *  Fixation at time=120 [Frame 1]: Is in active AOI -> a
     *  Fixation at time=140 [Frame 2]: Is not in active AOI -> #
     *  Fixation at time=150 [Frame 2]: Is in active AOI -> c
     *  Fixation at time=160 [Frame 3]: Is not in active AOI -> #
    **/
    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/02.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/02.tsv", exportData)) << "Error reading export file";

    exportData.erase(exportData.begin());
    std::string stringSequence01 = fixationToSequenceDynamic(exportData, dynamicAOIs, fpms);

    EXPECT_EQ(stringSequence01, "aba#c#") << "Error : Wrong sequence string for sequence 01.";
}

TEST(VALIDITY_CHECK, AOI_ELEMENT_ACTIVE_IN_MULTIPLE_FRAMES) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;
    double fpms = 8. / (180 - 100);

    /**
     *  Fixation at time=110 [Frame 1]: Is in active AOI -> a 
     *  Fixation at time=120 [Frame 2]: Is in active AOI -> a 
     *  Fixation at time=130 [Frame 3]: Is in active AOI -> b
     *  Fixation at time=140 [Frame 4]: Is not in active AOI -> b
     *  Fixation at time=150 [Frame 5]: Is in active AOI -> c
     *  Fixation at time=160 [Frame 6]: Is not in active AOI -> c
     *  Fixation at time=170 [Frame 7]: Is not in active AOI -> c
     *  Fixation at time=180 [Frame 8]: Is not in active AOI -> c
    **/
    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/03.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/03.tsv", exportData)) << "Error reading export file";

    exportData.erase(exportData.begin());
    std::string stringSequence01 = fixationToSequenceDynamic(exportData, dynamicAOIs, fpms);

    EXPECT_EQ(stringSequence01, "aabbcccc") << "Error : Wrong sequence string for sequence 01.";
}

TEST(BINARY_SEARCH_TEST, NORMAL) {

    AOIElement e1 = {0, 0, 0, 0, 1, 2};
    AOIElement e2 = {0, 0, 0, 0, 3, 3};
    AOIElement e3 = {0, 0, 0, 0, 4, 8};
    AOIElement e4 = {0, 0, 0, 0, 9, 9};
    AOIElement e5 = {0, 0, 0, 0, 10, 10};
    AOIElement e6 = {0, 0, 0, 0, 11, 99};
    AOIElement e7 = {0, 0, 0, 0, 100, 100};
    AOIElement e8 = {0, 0, 0, 0, 101, 101};

    std::vector<struct AOIElement> elements = {e1, e2, e3, e4, e5, e6, e7, e8};

    // Elements in list - Result should be the corresponding index
    EXPECT_EQ(findAOIPosition(elements, 2), 0);
    EXPECT_EQ(findAOIPosition(elements, 3), 1);
    EXPECT_EQ(findAOIPosition(elements, 6), 2);
    EXPECT_EQ(findAOIPosition(elements, 9), 3);
    EXPECT_EQ(findAOIPosition(elements, 10), 4);
    EXPECT_EQ(findAOIPosition(elements, 77), 5);
    EXPECT_EQ(findAOIPosition(elements, 100), 6);
    EXPECT_EQ(findAOIPosition(elements, 101), 7);

    // Element not list - Result should be -1
    EXPECT_EQ(findAOIPosition(elements, 0), -1);
    EXPECT_EQ(findAOIPosition(elements, 102), -1);

}

TEST(VALIDITY_CHECK, TEMPORAL_BINNING) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;
    double fpms = 3. / (130 - 100);

    /**
     *  We use temporal binning with bin-size of 50ms:
     *  Fixation at time=110 [Frame 1]: Is in active AOI for 50ms  -> a 
     *  Fixation at time=120 [Frame 2]: Is in active AOI for 40ms  -> b
     *  Fixation at time=130 [Frame 3]: Is in active AOI for 250ms -> ccccc
    **/
    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/04.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/04.tsv", exportData)) << "Error reading export file";

    exportData.erase(exportData.begin());
    std::string stringSequence01 = fixationToSequenceDynamic(exportData, dynamicAOIs, fpms, true, 50);

    EXPECT_EQ(stringSequence01, "abccccc") << "Error : Wrong sequence string for sequence 01.";
}

TEST(VALIDITY_CHECK, ARGUMENT_CHECK) {
    AOIImporter aoiImporter;
    std::vector<struct DynamicAOI> dynamicAOIs;

    TobiiImporter tobiiImporter;
    std::vector<struct TobiiExportData> exportData;
    struct GenericSourceInfo info;
    double fpms = 3. / (130 - 100);

    ASSERT_TRUE(aoiImporter.parseViperFormattedXML("res/01.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_TRUE(tobiiImporter.read("res/01.tsv", exportData)) << "Error reading export file";

    try {
        fixationToSequenceDynamic(exportData, dynamicAOIs, fpms, true, 0);
        FAIL() << "Bin size is zero but no exception is thrown !";
    } catch(const std::invalid_argument& ia) {
        // THIS IS OK AND EXPECTED
    } catch(...) {
        FAIL() << "Bin size is zero but wrong exception is thrown !";
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
