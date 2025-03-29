cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_COMPILER g++)
project(multi_user_chat_app)

file(GLOB_RECURSE SERVER_SOURCES "src/server/*.cpp")
file(GLOB_RECURSE CLIENT_SOURCES "src/client/*.cpp")

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

include_directories(include)
include_directories(lib/server_logging/include)
include_directories(lib/binary_protocol/include)
include_directories(lib/utils/include)

add_subdirectory(lib/server_logging)
add_subdirectory(lib/binary_protocol)
add_subdirectory(lib/utils)

add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

link_directories(${CMAKE_SOURCE_DIR}/lib/server_logging)
link_directories(${CMAKE_SOURCE_DIR}/lib/binary_protocol)
link_directories(${CMAKE_SOURCE_DIR}/lib/utils)

target_link_libraries(server PRIVATE server_logging)
target_link_libraries(server PRIVATE binary_protocol)
target_link_libraries(server PRIVATE utils)

target_link_libraries(client Qt6::Widgets)
