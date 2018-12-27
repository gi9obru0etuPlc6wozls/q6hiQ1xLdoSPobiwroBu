cmake_minimum_required(VERSION 3.10)
project(fs2_migration02)

set(CMAKE_CXX_STANDARD 11)

add_executable(fs2_migration02
        main.cpp
        Processor.cpp Processor.h
        Action.cpp Action.h
        YAMLtoJSON.cpp YAMLtoJSON.h
        file_exists.h
        )

target_link_libraries(fs2_migration02 yaml-cpp)

