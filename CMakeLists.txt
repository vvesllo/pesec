cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME pesec)
project(${PROJECT_NAME})

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/frontend/Lexer.cpp						include/frontend/Lexer.hpp
	src/frontend/Token.cpp						include/frontend/Token.hpp
	src/frontend/Parser.cpp						include/frontend/Parser.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})