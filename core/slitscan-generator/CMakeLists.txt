include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

file(GLOB SOURCES "*.cc")
add_library(gen_slitscan STATIC ${SOURCES})
target_link_libraries(gen_slitscan ${OpenCV_LIBS} tobii_importer)

enable_testing ()
#add_subdirectory (test)




