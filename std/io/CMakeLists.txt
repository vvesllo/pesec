cmake_minimum_required(VERSION 4.3)
project(io)

set(CMAKE_C_STANDARD 23)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/lib.c       include/lib.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})