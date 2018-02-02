add_executable(mtrajectory-unit-test test.cc)
target_link_libraries(mtrajectory-unit-test gtest_main mtrajectory)
add_test(mtrajectory-unit-test mtrajectory-unit-test)
