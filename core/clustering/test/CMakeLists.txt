add_executable(clustering-unit-test test.cc)
target_link_libraries(clustering-unit-test gtest_main hac)
add_test(clustering-unit-test clustering-unit-test)
