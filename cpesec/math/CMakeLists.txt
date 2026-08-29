cmake_minimum_required(VERSION 4.3)
project(math)

set(CMAKE_C_STANDARD 23)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/lib.c       include/lib.h
)

target_link_libraries(${PROJECT_NAME} m)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})