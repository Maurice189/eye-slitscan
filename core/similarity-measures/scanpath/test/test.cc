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
#include <functional>
#include <gtest/gtest.h>

#include <importer/eye-tracker-output/include/tobii_importer.h>
//#include <importer/aoi/include/aoi-importer.h>

#include "string_edit.h"

TEST(VALIDITY_CHECK, LEVENSHTEIN) {
    EXPECT_DOUBLE_EQ(levenshteinScore("", ""), 0.) << "Error at empty & empty";
    EXPECT_DOUBLE_EQ(levenshteinScore("", "similarity"), 0.) << "Error at empty & 'similarity'";
    EXPECT_DOUBLE_EQ(levenshteinScore("similarity", ""), 0.) << "Error at 'similarity' & empty";
    EXPECT_DOUBLE_EQ(levenshteinScore("slitscan", "similarity"), 0.2) << "Error at 'slitscan' & 'similarity'";
    EXPECT_DOUBLE_EQ(levenshteinScore("gauss", "poincare"), 0.) << "Error at 'gauss' & 'poincare'";
    EXPECT_DOUBLE_EQ(levenshteinScore("levenshtein", "levenshtein"), 1.) << "Error at 'levenshtein' & 'levenshtein'";
    EXPECT_DOUBLE_EQ(levenshteinScore("optimization", "optimi"), 0.5) << "Error at 'optimization' & 'optimi'";
    EXPECT_DOUBLE_EQ(levenshteinScore("optimi", "optimization"), 0.5) << "Error at 'opti' & 'optimization'";
}

TEST(VALIDITY_CHECK, NEEDLEMAN_WUNSCH) {

    std::function<double(char, char)> subWeights = [](char a, char b) {
        return a == b ? 1. : -1.;
    };

    EXPECT_DOUBLE_EQ(needlemanWunschScore("ACGTC", "", subWeights, 1., -1), -1) << "Error at ACGTC & empty";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("", "ACGTC", subWeights, 1., -1), -1) << "Error at empty & AGTC";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("", "", subWeights, 1., -1), -1) << "Error at empty & empty";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("ACGTC", "AGTC", subWeights, 1., -1), 3./5) << "Error at ACGTC & AGTC";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("AGTC", "ACGTC", subWeights, 1., -1), 3./5) << "Error at AGTC & ACGTC";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("CGTGAATTCAT", "GACTTAC", subWeights, 1., -1), -1./11) << "Error at CGTGAATTCAT & GACTTAC";

    /*
    subWeights = [](char a, char b) {
        return a == b ? 1. : 0;
    };

    EXPECT_DOUBLE_EQ(needlemanWunschScore("", "", subWeights, 1, 0), 0.) << "Error at empty & empty";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("", "similarity",subWeights, 1, 0), 0.) << "Error at empty & 'similarity'";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("similarity", "", subWeights, 1, 0), 0.) << "Error at 'similarity' & empty";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("slitscan", "similarity", subWeights, 1, 0), 0.2) << "Error at 'slitscan' & 'similarity'";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("gauss", "poincare", subWeights, 1, 0), 0.) << "Error at 'gauss' & 'poincare'";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("levenshtein", "levenshtein", subWeights, 1, 0), 1.) << "Error at 'levenshtein' & 'levenshtein'";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("optimization", "optimi", subWeights, 1, 0), 0.5) << "Error at 'optimization' & 'optimi'";
    EXPECT_DOUBLE_EQ(needlemanWunschScore("optimi", "optimization", subWeights, 1, 0), 0.5) << "Error at 'opti' & 'optimization'";
    */
}
/*

TEST(VALIDITY_CHECK, MULTIMATCH) {

    TobiiImporter tobiiImporter;

    std::vector<struct TobiiExportData> exportData01;
    std::vector<struct TobiiExportData> exportData02;

    if(!tobiiImporter.read("core/similarity-measures/scanpath/test/res/P1A-01.tsv", exportData01))
        return;

    if(!tobiiImporter.read("core/similarity-measures/scanpath/test/res/P2B-01.tsv", exportData02))
        return;

    int screenWidth = 1920;
    int screenHeight = 1080;

    double lengthThreshold = 0.1 * std::sqrt(std::pow(screenWidth, 2)+std::pow(screenHeight, 2));
    int durationThreshold = std::numeric_limits<int>::max();
    int angleThreshold = 75;

    struct Scanpath sp1 = generateScanpath(exportData01);
    struct Scanpath sp2 = generateScanpath(exportData02);

    struct MultiMatchInfo info;
    info.alignment = Alignment::POSITION;
    info.durationThreshold = durationThreshold;
    info.angleThreshold = angleThreshold;
    info.lengthThreshold = lengthThreshold;
    info.screenWidth = screenWidth;
    info.screenHeight = screenHeight;

    struct MultiMatchResult result = multiMatchScore(sp1, sp2, info);

}
*/
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
