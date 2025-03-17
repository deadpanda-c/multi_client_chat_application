cmake_minimum_required(VERSION 3.22)
project(multi_user_chat_app)

file(GLOB_RECURSE SERVER_SOURCES "src/server/*.cpp")
# file(GLOB_RECURSE CLIENT_SOURCES "src/client/*.cpp")
include_directories(include)

add_executable(server ${SERVER_SOURCES})
# add_executable(client ${CLIENT_SOURCES})
