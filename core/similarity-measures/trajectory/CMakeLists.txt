include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "*.cc")

add_library(mtrajectory STATIC ${SOURCES})
target_link_libraries(mtrajectory aoi_importer tobii_importer)

enable_testing ()
add_subdirectory (test)
