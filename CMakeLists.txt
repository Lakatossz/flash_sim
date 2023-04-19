cmake_minimum_required(VERSION 3.0)
project(flash_sim_project)

set(SOURCE_FILES main.cpp)

add_executable(flash_sim_run ${SOURCE_FILES})

include_directories(flash_sim)

add_subdirectory(flash_sim)
add_subdirectory(tests)

target_link_libraries(flash_sim_run flash_sim_)
