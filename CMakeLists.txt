cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME pesec)
project(${PROJECT_NAME})

add_executable(
	${PROJECT_NAME}
	src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})