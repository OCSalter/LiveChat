cmake_minimum_required(VERSION 3.22.0)
project(LiveChat VERSION 1.0.0)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GameNetworkingSockets/include"
    )

add_executable(LiveChat main.cpp)

target_link_libraries(LiveChat PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GameNetworkingSockets/bin/libGameNetworkingSockets.so")