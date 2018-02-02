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
#include "dtw.h"
#include "edr.h"
#include "frechet.h"
#include "util.h"


TEST(VALIDITY_CHECK, DTW_TEST) {

    struct Trajectory t01;
    struct Trajectory t02;
    struct Trajectory t03;
    struct Trajectory t04;

    t01.points = {std::pair<double, double>(1.2, 2.1),
                  std::pair<double, double>(3.0, 4.0),
                  std::pair<double, double>(5.23, 2.2),
                  std::pair<double, double>(1.8, 1.5)
                 };
    t02.points = {std::pair<double, double>(1.2, 3.2),
                  std::pair<double, double>(4.0, 3.0),
                  std::pair<double, double>(5.3, 6.4),
                  std::pair<double, double>(2.2,2.3)
                 };
    t03.points = {std::pair<double, double>(1.1, 2.2),
                  std::pair<double, double>(3.4, 4.2),
                 };

    t04.points = {std::pair<double, double>(-0.2, 2.2),
                  std::pair<double, double>(4.0, 4.0),
                  std::pair<double, double>(5.3, -2.2),
                  std::pair<double, double>(-1.2,1.3)
                 };

    // Standard - Same length
    EXPECT_NEAR(distanceDTW(t01, t02), 7.60, 0.01) << "Error : Wrong distance between P01 and P02";
    // Same length - Symetry
    EXPECT_NEAR(distanceDTW(t02, t01), 7.60, 0.01) << "Error : Wrong distance between P02 and P01";
    // Different length
    EXPECT_NEAR(distanceDTW(t01, t03), 6.43, 0.01) << "Error : Wrong distance between P01 and P03";
    // Different length - Symetry
    EXPECT_NEAR(distanceDTW(t03, t01), 6.43, 0.01) << "Error : Wrong distance between P03 and P01";
    // Negative signs
    EXPECT_NEAR(distanceDTW(t01, t04), 9.81, 0.01) << "Error : Wrong distance between P01 and P04";
    // Negative signs - Symetry
    EXPECT_NEAR(distanceDTW(t04, t01), 9.81, 0.01) << "Error : Wrong distance between P04 and P01";
    // One trajectory is empty
    t02.points.clear();
    EXPECT_NEAR(distanceDTW(t01, t02), 0.00, 0.01) << "Error : Wrong distance between P01 and P02";
    // One trajectory is empty
    t01.points.clear();
    EXPECT_NEAR(distanceDTW(t01, t03), 0.00, 0.01) << "Error : Wrong distance between P01 and P03";
    // Both trajectories are empty
    EXPECT_NEAR(distanceDTW(t01, t02), 0.00, 0.01) << "Error : Wrong distance between P01 and P02";
}


TEST(NORMALIZATION_TEST, SIMPLE) {

    struct Trajectory trajectory;

    trajectory.points = {std::pair<double, double>(1, 2),
                         std::pair<double, double>(-3, 7),
                         std::pair<double, double>(5, 2),
                         std::pair<double, double>(2, -1)
                        };

    std::vector<double> expectedFirst = {-0.087, -1.485, 1.310, 0.262};
    std::vector<double> expectedSecond = {-0.174, 1.566, -0.174, -1.218};

    struct NormalizedTrajectory normalized = normalizeTrajectory(trajectory);

    for(int i = 0; i < 4; i++) {
        EXPECT_NEAR(normalized.points[i].first, expectedFirst[i], 0.001) << "Error : Z-Normalisation at first component";
        EXPECT_NEAR(normalized.points[i].second, expectedSecond[i], 0.001) << "Error : Z-Normalisation at second component";
    }
}

TEST(VALIDITY_CHECK, FRECHET_TEST) {

    struct Trajectory t01;
    struct Trajectory t02;

    t01.points = {std::pair<double, double>(64., 25.),
                  std::pair<double, double>(42., 55.),
                  std::pair<double, double>(37., 21.),
                  std::pair<double, double>(34., 76.),
                  std::pair<double, double>(77., 2.),
                  std::pair<double, double>(98., 0.),
                  std::pair<double, double>(9., 20.),
                  std::pair<double, double>(20., 10.),
                  std::pair<double, double>(12., 27.),
                  std::pair<double, double>(32., 61.),
                  std::pair<double, double>(88., 49.),
                  std::pair<double, double>(60., 90.),
                  std::pair<double, double>(99., 37.),
                  std::pair<double, double>(85., 53.),
                  std::pair<double, double>(7., 87.),
                  std::pair<double, double>(67., 33.),
                  std::pair<double, double>(20., 62.),
                  std::pair<double, double>(4., 88.)
                 };
    t02.points = {std::pair<double, double>(76., 92.),
                  std::pair<double, double>(71., 59.),
                  std::pair<double, double>(65., 73.),
                  std::pair<double, double>(29., 52.),
                  std::pair<double, double>(19., 13.),
                  std::pair<double, double>(81., 6.),
                  std::pair<double, double>(89., 36.),
                  std::pair<double, double>(76., 10.),
                  std::pair<double, double>(38., 93.),
                  std::pair<double, double>(60., 44.),
                  std::pair<double, double>(5., 26.),
                  std::pair<double, double>(58., 84.),
                  std::pair<double, double>(46., 16.),
                  std::pair<double, double>(0., 55.),
                  std::pair<double, double>(56., 71.)
                 };
    // Standard - Same length
    EXPECT_EQ(discreteFrechetDistance(t01, t02), 79) << "Error : Wrong distance between P01 and P02";
}

/*
TEST(VALIDITY_CHECK, EDR_TEST) {

    struct Trajectory t01;
    struct Trajectory t02;
    struct Trajectory t03;
    struct Trajectory t04;

    t01.points = {std::pair<double, double>(1.2, 0.0),
                  std::pair<double, double>(3.0, 0.0),
                  std::pair<double, double>(5.23, 0.0),
                  std::pair<double, double>(1.8, 0.0)
                 };
    t02.points = {std::pair<double, double>(1.2, 0.0),
                  std::pair<double, double>(4.0, 0.0),
                  std::pair<double, double>(5.3, 0.0),
                  std::pair<double, double>(2.2, 0.0)
                 };
    t03.points = {std::pair<double, double>(1.1, 0.0),
                  std::pair<double, double>(3.4, 0.0),
                 };

    t04.points = {std::pair<double, double>(-0.2, 0.0),
                  std::pair<double, double>(4.0, 0.0),
                  std::pair<double, double>(5.3, 0.0),
                  std::pair<double, double>(-1.2,0.0)
                 };

    // Standard - Same length
    EXPECT_EQ(distanceEDR(t01, t02, 0.1), 2) << "Error : Wrong distance between P01 and P02";
    // Same length - Symetry
    EXPECT_EQ(distanceEDR(t02, t01, 0.1), 2) << "Error : Wrong distance between P02 and P01";
    // Different length
    EXPECT_EQ(distanceEDR(t01, t03, 0.1), 3) << "Error : Wrong distance between P01 and P03";
    // Different length - Symetry
    EXPECT_EQ(distanceEDR(t03, t01, 0.1), 3) << "Error : Wrong distance between P03 and P01";
    // Negative signs
    EXPECT_EQ(distanceEDR(t01, t04, 0.1), 3) << "Error : Wrong distance between P01 and P04";
    // Negative signs - Symetry
    EXPECT_EQ(distanceEDR(t04, t01, 0.1), 3) << "Error : Wrong distance between P04 and P01";
    // One trajectory is empty
    t02.points.clear();
    EXPECT_EQ(distanceEDR(t01, t02, 0.1), 4) << "Error : Wrong distance between P01 and P02";
    // One trajectory is empty
    t01.points.clear();
    EXPECT_EQ(distanceEDR(t01, t03, 0.1), 2) << "Error : Wrong distance between P01 and P03";
    // Both trajectories are empty
    EXPECT_EQ(distanceEDR(t01, t02, 0.1), 0) << "Error : Wrong distance between P01 and P02";
}
*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
