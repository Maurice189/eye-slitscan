add_executable(sequence-gen-unit-test test.cc)
target_link_libraries(sequence-gen-unit-test gtest gtest_main sequence_gen)
add_test(sequence-gen-unit-test sequence-gen-unit-test)
