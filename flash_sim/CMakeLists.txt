cmake_minimum_required(VERSION 3.0)
project(flash_sim)

include_directories(include)

add_executable(
        flash_sim
        ../../main.cpp
        source/flash/flash.cpp
        source/flash/statistics/Page_Stats.cpp
        source/flash/statistics/Block_Stats.cpp
        source/flash/statistics/Memory_Stats.cpp source/flash/flash.cpp)
