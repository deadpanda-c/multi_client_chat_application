cmake_minimum_required(VERSION 3.22)
project(utils)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE MAIN "sample/main.cpp")
include_directories(include)

option(BUILD_EXEC "Build the executable" OFF)

if (BUILD_EXEC)
  add_executable(utils_exec ${MAIN} ${SOURCES})
endif()


add_library(utils ${SOURCES})



