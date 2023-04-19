cmake_minimum_required(VERSION 3.0)
project("Gtest")

find_package(GTest REQUIRED)

add_executable(a.out flash_sim_check.cpp)
target_link_libraries(a.out ${GTEST_LIBRARIES})

enable_testing()
add_test(add a.out)
