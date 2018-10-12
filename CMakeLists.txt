cmake_minimum_required(VERSION 3.10)
project(fs2_template02)

set(CMAKE_CXX_STANDARD 11)

add_executable(fs2_migration01 main.cpp)

target_link_libraries(fs2_migration01 yaml-cpp)