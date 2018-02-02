cmake_minimum_required(VERSION 2.8)

set (SOURCES
    histo-measure.cc
    image-measure.cc
    QtOpenCV/cvmatandqimage.cpp
    )

find_package(OpenCV REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

add_definitions( ${QT_DEFINITIONS} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})

add_library(mimage STATIC ${SOURCES})
target_link_libraries(mimage Qt5::Core Qt5::Gui ${OpenCV_LIBS})

enable_testing ()
#add_subdirectory (test)
