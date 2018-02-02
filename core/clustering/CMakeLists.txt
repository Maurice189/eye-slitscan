include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "*.cc")
add_library(hac STATIC ${SOURCES})

enable_testing ()
add_subdirectory (test)
