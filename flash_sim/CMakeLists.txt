cmake_minimum_required(VERSION 3.24.0)
project(flash_sim)

include_directories(include)

add_executable(
        flash_sim
        ../tests/main_test.cpp
        source/flash/flash.cpp
        source/flash/statistics/Page_Stats.cpp
        source/flash/statistics/Block_Stats.cpp
        source/flash/statistics/Memory_Stats.cpp source/flash/flash.cpp)


include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(flash_sim PRIVATE nlohmann_json::nlohmann_json)
