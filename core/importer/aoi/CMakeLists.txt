include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/tinyxml2)

file(GLOB SOURCES "*.cc" "tinyxml2/tinyxml2.cpp")
add_library(aoi_importer STATIC ${SOURCES})

enable_testing ()
add_subdirectory (test)
