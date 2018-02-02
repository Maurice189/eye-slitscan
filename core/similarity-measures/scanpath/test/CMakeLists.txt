add_executable(mscanpath-unit-test test.cc)
target_link_libraries(mscanpath-unit-test gtest_main mscanpath sequence_gen)
add_test(mscanpath-unit-test mscanpath-unit-test)
