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
#include <utility>
#include "hac.h"
#include "../copheneticcorrelation.h"

#define CL_ABS_ERROR 0.01

TEST(CORPHENETIC_CORRELATION, SIMPLE) {
    auto l1 = Cluster(1, "1");
    auto l2 = Cluster(2, "2");
    auto l3 = Cluster(3, "3");
    auto l4 = Cluster(4, "4");
    auto l5 = Cluster(5, "5");
    auto l6 = Cluster(6, "6");

    auto _1u2 = l1.merge(l2, 0.91);
    auto _3u4 = l3.merge(l4, 0.81);
    auto _5u6 = l5.merge(l6, 0.41);

    auto _1u2_3u4 = _1u2.merge(_3u4, 0.71);
    auto root = _1u2_3u4.merge(_5u6, 0.11);

    ClusteringResult result;

    result.distances = {0.91, 0.81, 0.41, 0.71, 0.11};
    result.tree = root.tree();

    CopheneticCorrelation corr(result, result, {"1", "2", "3", "4", "5", "6"}, 10);

    std::vector<int> values = corr.copheneticValuesR1();
    std::vector<int> expected = {10, 7, 7, 1, 1, 7, 7, 1, 1, 8, 1, 1, 1, 1, 4};

    EXPECT_TRUE(values == expected) << "Wrong cophenetic values or wrong order";
}

static bool stepCheck(std::pair<int, int> result, std::pair<int, int> expected)
{
    return  (result.first == expected.first && result.second == expected.second) ||
            (result.first == expected.second && result.second == expected.first);
}

TEST(HAC_SINGLE_LINKAGE, SIMPLE) {

    Clustering clustering({{0, 2, 5, 7},
                           {2, 0, 7, 6},
                           {5, 7, 0, 1},
                           {7, 6, 1, 0}}, {"a", "b", "c", "d"});
    
    struct ClusteringResult result = clustering.executeHAC();

    ASSERT_TRUE(result.steps.size() == 3) << "Error result steps has wrong size";
    ASSERT_TRUE(result.distances.size() == 3) << "Error result distances has wrong size";

    EXPECT_TRUE(stepCheck(result.steps[0], {2,3})) << "Error : Step 1 wrong";
    EXPECT_TRUE(stepCheck(result.steps[1], {0,1})) << "Error : Step 2 wrong";
    EXPECT_TRUE(stepCheck(result.steps[2], {0,1})) << "Error : Step 3 wrong";

    EXPECT_EQ(result.distances[0], 1) << "Error : Distance 1 wrong";
    EXPECT_EQ(result.distances[1], 2) << "Error : Distance 2 wrong";
    EXPECT_EQ(result.distances[2], 5) << "Error : Distance 3 wrong";
}

TEST(HAC_SINGLE_LINKAGE, CHAINING) {
    Clustering clustering({{0,  2,  5, 9, 14},
                           {2,  0,  3, 7, 12},
                           {5,  3,  0, 4, 9},
                           {9,  7,  4, 0, 5},
                           {14, 12, 9, 5, 0}} , {"a", "b", "c", "d", "e"});
    
    struct ClusteringResult result = clustering.executeHAC();

    ASSERT_TRUE(result.steps.size() == 4) << "Error result steps has wrong size";
    ASSERT_TRUE(result.distances.size() == 4) << "Error result distances has wrong size";

    EXPECT_TRUE(stepCheck(result.steps[0], {0,1})) << "Error : Step 1 wrong";
    EXPECT_TRUE(stepCheck(result.steps[1], {0,3})) << "Error : Step 2 wrong";
    EXPECT_TRUE(stepCheck(result.steps[2], {0,2})) << "Error : Step 3 wrong";
    EXPECT_TRUE(stepCheck(result.steps[3], {0,1})) << "Error : Step 4 wrong";

    EXPECT_EQ(result.distances[0], 2) << "Error : Distance 1 wrong";
    EXPECT_EQ(result.distances[1], 3) << "Error : Distance 2 wrong";
    EXPECT_EQ(result.distances[2], 4) << "Error : Distance 3 wrong";
    EXPECT_EQ(result.distances[3], 5) << "Error : Distance 4 wrong";

}

TEST(HAC, INPUT_CHECK) {

    // Illegal Dimensions - Check
    try {
        Clustering clustering({{0,  2,  5, 9, 14},
                               {2,  0,  3, 7, 12},
                               {5,  3,  0, 4, 9},
                               {14, 12, 9, 5, 0}}, {"a", "b", "c", "d", "e"});

        ASSERT_TRUE(false) << "Error : Dimensions";        
    } catch (const std::invalid_argument &ex) {
        ASSERT_TRUE(true);
    }

    // Non-symetry - Check
    try {
        Clustering clustering({{0,  2,  5, 9, 14},
                               {2,  0,  3, 7, 12},
                               {5,  3,  0, 4, 9},
                               {-9,  7,  4, 0, 5},
                               {14, 12, 9, 5, 0}}, {"a", "b", "c", "d", "e"});

        ASSERT_TRUE(false) << "Error : Symetry";        
    } catch (const std::invalid_argument &ex) {
        ASSERT_TRUE(true);
    }

    // Emptyness - Check
    try {
        std::vector<std::vector<double>> emptySimilarityMatrix;
        Clustering clustering(emptySimilarityMatrix, {""});

        ASSERT_TRUE(false) << "Error : Empty";        
    } catch (const std::invalid_argument &ex) {
        ASSERT_TRUE(true);
    }

}


TEST(HAC_SINGLE_LINKAGE, EXTENDED) {

    Clustering clustering ({{0.0, 0.5, 32.0, 13.0, 18.0, 10.25}, 
                            {0.5, 0.0, 24.5, 8.5, 12.5, 6.25}, 
                            {32.0, 24.5, 0.0, 5.0, 2.0, 6.25}, 
                            {13.0, 8.5, 5.0, 0.0, 1.0, 0.25}, 
                            {18.0, 12.5, 2.0, 1.0, 0.0, 1.25}, 
                            {10.25, 6.25, 6.25, 0.25, 1.25, 0.0}}, {"a", "b", "c", "d", "e", "f"});
    
    struct ClusteringResult result = clustering.executeHAC();

    ASSERT_TRUE(result.steps.size() == 5) << "Error result steps has wrong size";
    ASSERT_TRUE(result.distances.size() == 5) << "Error result distances has wrong size";

    EXPECT_TRUE(stepCheck(result.steps[0], {3,5})) << "Error : Step 1 wrong";
    EXPECT_TRUE(stepCheck(result.steps[1], {0,1})) << "Error : Step 2 wrong";
    EXPECT_TRUE(stepCheck(result.steps[2], {1,2})) << "Error : Step 3 wrong";
    EXPECT_TRUE(stepCheck(result.steps[3], {0,2})) << "Error : Step 4 wrong";
    EXPECT_TRUE(stepCheck(result.steps[4], {0,1})) << "Error : Step 5 wrong";

    
    EXPECT_NEAR(result.distances[0], 0.25, CL_ABS_ERROR) << "Error : Distance 1 wrong";
    EXPECT_NEAR(result.distances[1], 0.5, CL_ABS_ERROR) << "Error : Distance 2 wrong";
    EXPECT_NEAR(result.distances[2], 1.0, CL_ABS_ERROR) << "Error : Distance 3 wrong";
    EXPECT_NEAR(result.distances[3], 2.0, CL_ABS_ERROR) << "Error : Distance 4 wrong";
    EXPECT_NEAR(result.distances[4], 6.25, CL_ABS_ERROR) << "Error : Distance 5 wrong";

}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

