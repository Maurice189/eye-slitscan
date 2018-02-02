add_executable(tobii_importer-unit-test test.cc)
target_link_libraries(tobii_importer-unit-test gtest_main tobii_importer)
add_test(tobii_importer-unit-test tobii_importer-unit-test)
