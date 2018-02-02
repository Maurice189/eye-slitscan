include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "*.cc")

add_library(sequence_gen STATIC ${SOURCES})
target_link_libraries(sequence_gen aoi_importer tobii_importer)

enable_testing ()
add_subdirectory (test)
