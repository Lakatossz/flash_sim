cmake_minimum_required(VERSION 3.24.0)
project(flash_sim_test)

add_executable(
        flash_sim_test
        main_test.cpp)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(flash_sim PRIVATE nlohmann_json::nlohmann_json)
