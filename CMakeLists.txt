cmake_minimum_required(VERSION 4.3)
project(pesec C)

set(CMAKE_C_STANDARD 23)

add_executable(pesec
        src/main.c

        src/ast/node.c
        src/ast/literal_node.c
        src/ast/binary_node.c

        src/frontend/token.c
        src/frontend/lexer.c
        src/frontend/parser.c

        src/common/string_view.c
        src/common/panic.c
        src/common/alloc.c
        src/runtime/variable_table.c
        include/runtime/variable_table.h
)

target_include_directories(pesec PRIVATE ${CMAKE_SOURCE_DIR})