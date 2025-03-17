cmake_minimum_required(VERSION 3.22)
project(server_logging)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE MAIN "sample/main.cpp")
include_directories(include)

add_executable(server_logging ${SOURCES} ${MAIN})
add_library(server_logging_lib ${SOURCES})


