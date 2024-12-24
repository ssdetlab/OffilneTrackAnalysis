cmake_minimum_required(VERSION 3.14)

project(OfflineAnalysis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wl,--no-as-needed")

set(DictLib /home/romanurmanov/lab/LUXE/acts_tracking/TrackingPipeline_build/lib/libSimEventDict.so)
message(STATUS "DictLib: ${DictLib}")

find_package(
    ROOT 
    COMPONENTS 
    Core
    Hist
    HistPainter
    RIO 
    Tree
    Physics
    REQUIRED)

add_executable(
    offlineAnalysis 
    main.cpp)

target_include_directories(
    offlineAnalysis
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(
    offlineAnalysis
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    ${DictLib})
