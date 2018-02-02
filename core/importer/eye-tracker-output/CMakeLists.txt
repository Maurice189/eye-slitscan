include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/tinyxml2)

file(GLOB SOURCES "*.cc")
add_library(tobii_importer STATIC ${SOURCES})

enable_testing ()
add_subdirectory (test)


