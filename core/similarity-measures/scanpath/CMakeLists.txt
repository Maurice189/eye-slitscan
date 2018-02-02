include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "*.cc")

add_library(mscanpath STATIC ${SOURCES})
target_link_libraries(mscanpath aoi_importer tobii_importer)

enable_testing ()
add_subdirectory (test)
