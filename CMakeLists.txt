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
	
	src/frontend/ast/ASTNode.cpp				include/frontend/ast/ASTNode.hpp
	src/frontend/ast/Value.cpp					include/frontend/ast/Value.hpp
	src/frontend/ast/IntegerNode.cpp			include/frontend/ast/IntegerNode.hpp
	src/frontend/ast/StringNode.cpp				include/frontend/ast/StringNode.hpp
	src/frontend/ast/BinaryOpNode.cpp			include/frontend/ast/BinaryOpNode.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})