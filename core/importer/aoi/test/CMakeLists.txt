add_executable(aoi_importer-unit-test test.cc)
target_link_libraries(aoi_importer-unit-test gtest_main aoi_importer)
add_test(aoi_importer-unit-test aoi_importer-unit-test)
