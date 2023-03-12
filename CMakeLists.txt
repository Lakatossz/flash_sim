cmake_minimum_required(VERSION 3.24)
project(flash_sim)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(
        flash_sim
        source/main.cpp
        source/flash/flash.cpp)
