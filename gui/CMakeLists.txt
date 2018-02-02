cmake_minimum_required(VERSION 2.6)
# declaring files in your project

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set ( SOURCES
    #main.cc
    #mainwindow.cc
    #slitscanitem.cc
    #frameslideritem.cc
    #measuretreeitem.cc
    #dendrogramitem.cc
    #slitscanview.cc
    #dendrogramview.cc
    #timespanselector.cc
    #timespanhandle.cc
    #historylistitem.cc
    #slitscanvideo.cc
    #similaritymatrixview.cc
    #videooverlaywidget.cc
    #screenshot.cc
    #csvexport.cc
    #historylistwidget.cc
    #)


file(GLOB SOURCES "*.cc")

set ( MOC_HEADERS
    mainwindow.h
    frameslideritem.h
    slitscanview.h
    slitscanitem.h
    dendrogramview.h
    dendrogramitem.h
    interactivelineitem.h
    timespanselector.h
    timespanhandle.h
    historylistitem.h
    slitscanvideo.h
    similaritymatrixview.h
    screenshot.h
    historylistwidget.h
    representationview.h
    )

set (UIS mainwindow.ui)


find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Charts)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOUIC ON)

QT5_WRAP_CPP(app_hdr_moc ${MOC_HEADERS})
QT5_WRAP_UI(app_form_hdr mainwindow.ui)
QT5_ADD_RESOURCES(RESOURCES style/style.qrc)

add_library(mainwindow ${app_hdr_moc} ${app_form_hdr})
qt5_use_modules(mainwindow Widgets)

add_executable(app ${SOURCES} mainwindow ${RESOURCES})
target_include_directories(app PUBLIC ../core)
target_link_libraries(app ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} aoi_importer tobii_importer sequence_gen mimage mscanpath mtrajectory gen_slitscan hac ${CMAKE_THREAD_LIBS_INIT})
qt5_use_modules(app Core Gui Widgets Multimedia MultimediaWidgets Charts)
