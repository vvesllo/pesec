cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME pesec)
project(${PROJECT_NAME})

add_executable(
	${PROJECT_NAME}
	src/main.cpp

	src/frontend/Token.cpp									include/frontend/Token.hpp
	src/frontend/Lexer.cpp									include/frontend/Lexer.hpp
	src/frontend/Parser.cpp									include/frontend/Parser.hpp
	src/frontend/Value.cpp									include/frontend/Value.hpp
	src/frontend/Context.cpp								include/frontend/Context.hpp
	src/frontend/FunctionValue.cpp							include/frontend/FunctionValue.hpp
	src/frontend/LibraryLoader.cpp							include/frontend/LibraryLoader.hpp

	src/frontend/ast/ASTNode.cpp							include/frontend/ast/ASTNode.hpp
	src/frontend/ast/BlockNode.cpp							include/frontend/ast/BlockNode.hpp
	src/frontend/ast/LiteralNode.cpp						include/frontend/ast/LiteralNode.hpp
	src/frontend/ast/BinaryOpNode.cpp						include/frontend/ast/BinaryOpNode.hpp
	src/frontend/ast/BreakNode.cpp							include/frontend/ast/BreakNode.hpp
	src/frontend/ast/BreakValueException.cpp				include/frontend/ast/BreakValueException.hpp
	src/frontend/ast/ReturnNode.cpp							include/frontend/ast/ReturnNode.hpp
	src/frontend/ast/ReturnValueException.cpp				include/frontend/ast/ReturnValueException.hpp
	
	src/frontend/ast/UseNode.cpp							include/frontend/ast/UseNode.hpp

	src/frontend/ast/WhileNode.cpp							include/frontend/ast/WhileNode.hpp
	src/frontend/ast/IfNode.cpp								include/frontend/ast/IfNode.hpp
	
	src/frontend/ast/FunctionNode.cpp						include/frontend/ast/FunctionNode.hpp
	src/frontend/ast/FunctionCallNode.cpp					include/frontend/ast/FunctionCallNode.hpp
	src/frontend/ast/FunctionInstantCallNode.cpp			include/frontend/ast/FunctionInstantCallNode.hpp
	
	src/frontend/ast/VariableNode.cpp						include/frontend/ast/VariableNode.hpp
	src/frontend/ast/VariableDefinitionNode.cpp				include/frontend/ast/VariableDefinitionNode.hpp
	src/frontend/ast/VariableAssignmentNode.cpp				include/frontend/ast/VariableAssignmentNode.hpp
	src/frontend/ast/VariableCompoundAssignmentNode.cpp		include/frontend/ast/VariableCompoundAssignmentNode.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

add_subdirectory(stdlib/io)
add_subdirectory(stdlib/math)