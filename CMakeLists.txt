cmake_minimum_required(VERSION 4.3)
project(pesec C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

add_executable(pesec
        src/main.c

        src/ast/node.c
        src/ast/literal_node.c
        src/ast/binary_node.c
        src/ast/unary_node.c
        src/ast/variable_node.c
        src/ast/call_node.c
        src/ast/assignment_node.c
        src/ast/sequence_node.c
        src/ast/definition_node.c
        src/ast/conditional_node.c
        src/ast/function_node.c
        src/ast/vector_node.c
        src/ast/object_node.c
        src/ast/index_access_node.c
        src/ast/field_access_node.c
        src/ast/range_node.c
        src/ast/for_node.c
        src/ast/while_node.c
        src/ast/control_flow_node.c

        src/frontend/token.c
        src/frontend/lexer.c
        src/frontend/parser.c

        src/common/string_view.c
        src/common/panic.c
        src/common/alloc.c
        src/common/parameter_vector.c
        src/common/node_vector.c
        src/common/file_read.c

        src/runtime/environment.c
        src/runtime/variable_table.c
        src/runtime/value.c
        src/runtime/function_value.c
        src/runtime/vector_value.c
        src/runtime/object_value.c
)

target_link_libraries(pesec PRIVATE m)

target_include_directories(pesec PRIVATE ${CMAKE_SOURCE_DIR})