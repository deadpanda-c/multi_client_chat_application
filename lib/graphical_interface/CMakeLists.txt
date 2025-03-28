cmake_minimum_required(VERSION 3.22)
project(graphical_interface)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE MAIN "sample/main.cpp")
include_directories(include)

option(BUILD_EXEC "Build the executable" OFF)

if (BUILD_EXEC)
  add_executable(graphical_interface_sample ${MAIN} ${SOURCES})
  target_link_libraries(graphical_interface_sample Qt6::Widgets)
endif()

add_library(graphical_interface SHARED ${SOURCES})
target_link_libraries(graphical_interface Qt6::Widgets)

