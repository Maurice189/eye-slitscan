add_subdirectory(util)
add_subdirectory(scanpath)
add_subdirectory(image)
add_subdirectory(trajectory)

target_include_directories(mscanpath-unit-test PRIVATE .)
