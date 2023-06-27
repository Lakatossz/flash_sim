cmake_minimum_required(VERSION 3.24.0)
project(flash_sim)

include_directories(include)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

include_directories("${json_SOURCE_DIR}/include")
set(JSON_Include_Dir "${json_SOURCE_DIR}/include" CACHE INTERNAL "")

add_library(
        flash_sim
        source/flash/flash.cpp
        source/flash/statistics/Page_Stats.cpp
        source/flash/statistics/Block_Stats.cpp
        source/flash/statistics/Memory_Stats.cpp source/flash/flash.cpp)

target_link_libraries(flash_sim PRIVATE nlohmann_json::nlohmann_json)

# If you only include this third party in PRIVATE source files, you do not need to install it
# when your main project gets installed.
set(JSON_Install OFF CACHE INTERNAL "")