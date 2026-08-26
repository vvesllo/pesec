cmake_minimum_required(VERSION 4.3)
project(network)

set(CMAKE_C_STANDARD 23)

find_package(CURL REQUIRED)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/lib.c       include/lib.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})