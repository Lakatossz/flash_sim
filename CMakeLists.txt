cmake_minimum_required(VERSION 3.24.0)
project(flash_sim_project)

set(SOURCE_FILES main.cpp test_simulation/Test_Simulation.cpp test_simulation/test/test.cpp)

include_directories(flash_sim)

add_subdirectory(flash_sim)

include_directories("${JSON_Include_Dir}")

add_executable(flash_sim_run ${SOURCE_FILES})

target_link_libraries(flash_sim_run flash_sim)
