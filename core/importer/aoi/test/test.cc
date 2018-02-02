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
#include "aoi-importer.h"

TEST(VALIDITY_CHECK, FILE_OPEN_TEST) {
    AOIImporter importer;
    std::vector<struct DynamicAOI> dynamicAOIs;
    struct GenericSourceInfo info;

    ASSERT_TRUE(importer.parseViperFormattedXML("01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_FALSE(importer.parseViperFormattedXML("non.xml", dynamicAOIs, info)) << "Error reading export file";
}

TEST(VALIDITY_CHECK, FILE_FORMAT_TEST) {
    AOIImporter importer;
    std::vector<struct DynamicAOI> dynamicAOIs;
    struct GenericSourceInfo info;

    ASSERT_TRUE(importer.parseViperFormattedXML("01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_FALSE(importer.parseViperFormattedXML("01-malformed.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_FALSE(importer.parseViperFormattedXML("02-malformed.xml", dynamicAOIs, info)) << "Error reading export file";
}

TEST(VALIDITY_CHECK, SOURCE_INFO_TEST) {
    AOIImporter importer;
    std::vector<struct DynamicAOI> dynamicAOIs;
    struct GenericSourceInfo info;

    ASSERT_TRUE(importer.parseViperFormattedXML("01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";
    EXPECT_EQ(info.width, 1920) << "Error : Wrong source width.";
    EXPECT_EQ(info.height, 1080) << "Error : Wrong source height.";
    EXPECT_EQ(info.numberOfFrames, 625) << "Error : Wrong number of frames.";
}

TEST(DATA_CHECK, BBOX_TEST) {

    AOIImporter importer;
    std::vector<struct DynamicAOI> dynamicAOIs;
    struct GenericSourceInfo info;

    ASSERT_TRUE(importer.parseViperFormattedXML("01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";

    ///////////////////////////////////////////////////////////////
    //                        1. OBJECT                          //
    ///////////////////////////////////////////////////////////////
    ASSERT_EQ(dynamicAOIs[0].elements.size(), (unsigned int) 625);

    EXPECT_EQ(dynamicAOIs[0].elements[0].width, 74) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[0].elements[0].x, -8) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[0].elements[0].y, 355) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[0].elements[0].height, 131) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[0].elements[0].framespanStart, 1) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[0].elements[0].framespanEnd, 1) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[0].elements[1].width, 82) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[0].elements[1].x, -8) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[0].elements[1].y, 346) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[0].elements[1].height, 140) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[0].elements[1].framespanStart, 2) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[0].elements[1].framespanEnd, 2) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[0].elements[2].width, 91) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[0].elements[2].x, -8) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[0].elements[2].y, 337) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[0].elements[2].height, 149) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[0].elements[2].framespanStart, 3) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[0].elements[2].framespanEnd, 3) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[0].elements.back().width, 310) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[0].elements.back().x, -14) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[0].elements.back().y, 250) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[0].elements.back().height, 251) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[0].elements.back().framespanStart, 625) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[0].elements.back().framespanEnd, 625) << "Error : Wrong framespanEnd.";

    ///////////////////////////////////////////////////////////////
    //                        2. OBJECT                          //
    ///////////////////////////////////////////////////////////////

    ASSERT_EQ(dynamicAOIs[1].elements.size(), (unsigned int) 31);

    EXPECT_EQ(dynamicAOIs[1].elements[0].width, 190) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[1].elements[0].x, 1874) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[1].elements[0].y, 455) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[1].elements[0].height, 160) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[1].elements[0].framespanStart, 57) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[1].elements[0].framespanEnd, 57) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[1].elements[1].width, 263) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[1].elements[1].x, 1796) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[1].elements[1].y, 437) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[1].elements[1].height, 177) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[1].elements[1].framespanStart, 58) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[1].elements[1].framespanEnd, 58) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[1].elements[2].width, 337) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[1].elements[2].x, 1717) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[1].elements[2].y, 419) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[1].elements[2].height, 193) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[1].elements[2].framespanStart, 59) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[1].elements[2].framespanEnd, 59) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[1].elements.back().width, 194) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[1].elements.back().x, -46) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[1].elements.back().y, 275) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[1].elements.back().height, 327) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[1].elements.back().framespanStart, 87) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[1].elements.back().framespanEnd, 87) << "Error : Wrong framespanEnd.";

    ///////////////////////////////////////////////////////////////
    //                        3. OBJECT                          //
    ///////////////////////////////////////////////////////////////

    ASSERT_EQ(dynamicAOIs[2].elements.size(), (unsigned int) 38);

    EXPECT_EQ(dynamicAOIs[2].elements[0].width, 103) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[2].elements[0].x, 0) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[2].elements[0].y, 397) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[2].elements[0].height, 201) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[2].elements[0].framespanStart, 553) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[2].elements[0].framespanEnd, 553) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[2].elements[1].width, 176) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[2].elements[1].x, 0) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[2].elements[1].y, 387) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[2].elements[1].height, 228) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[2].elements[1].framespanStart, 554) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[2].elements[1].framespanEnd, 554) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[2].elements[2].width, 248) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[2].elements[2].x, 0) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[2].elements[2].y, 377) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[2].elements[2].height, 254) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[2].elements[2].framespanStart, 555) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[2].elements[2].framespanEnd, 555) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[2].elements.back().width, 175) << "Error : Wrong width.";
    EXPECT_EQ(dynamicAOIs[2].elements.back().x, 1826) << "Error : Wrong x.";
    EXPECT_EQ(dynamicAOIs[2].elements.back().y, 280) << "Error : Wrong y.";
    EXPECT_EQ(dynamicAOIs[2].elements.back().height, 437) << "Error : Wrong height.";
    EXPECT_EQ(dynamicAOIs[2].elements.back().framespanStart, 590) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[2].elements.back().framespanEnd, 590) << "Error : Wrong framespanEnd.";
}

TEST(DATA_CHECK, OBJECT_TEST) {

    AOIImporter importer;
    std::vector<struct DynamicAOI> dynamicAOIs;
    struct GenericSourceInfo info;

    ASSERT_TRUE(importer.parseViperFormattedXML("01-car pursuit.xml", dynamicAOIs, info)) << "Error reading export file";
    ASSERT_EQ(dynamicAOIs.size(), (unsigned int) 3) << "Wrong number of objects";

    EXPECT_EQ(dynamicAOIs[0].id, "0") << "Error : Wrong id.";
    EXPECT_EQ(dynamicAOIs[0].name, "Red Car") << "Error : Wrong name.";
    EXPECT_EQ(dynamicAOIs[0].framespanStart, 1) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[0].framespanEnd, 625) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[1].id, "1") << "Error : Wrong id.";
    EXPECT_EQ(dynamicAOIs[1].name, "White Car") << "Error : Wrong name.";
    EXPECT_EQ(dynamicAOIs[1].framespanStart, 57) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[1].framespanEnd, 87) << "Error : Wrong framespanEnd.";

    EXPECT_EQ(dynamicAOIs[2].id, "2") << "Error : Wrong id.";
    EXPECT_EQ(dynamicAOIs[2].name, "White Car") << "Error : Wrong name.";
    EXPECT_EQ(dynamicAOIs[2].framespanStart, 553) << "Error : Wrong framespanStart.";
    EXPECT_EQ(dynamicAOIs[2].framespanEnd, 590) << "Error : Wrong framespanEnd.";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
