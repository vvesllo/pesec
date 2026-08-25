cmake_minimum_required(VERSION 4.3)
project(types)

set(CMAKE_CXX_STANDARD 23)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/lib.cpp         include/lib.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})