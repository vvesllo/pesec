cmake_minimum_required(VERSION 4.3)
project(pesec)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -g")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")

add_executable(
        ${PROJECT_NAME}
        src/main.c
        src/context.c                               include/context.h
        src/lexer.c                                 include/lexer.h
        src/token.c                                 include/token.h
        src/parser.c                                include/parser.h
        src/value.c                                 include/value.h
        src/number_value.c                          include/number_value.h
        src/string_value.c                          include/string_value.h
        src/function_value.c                        include/function_value.h
        src/structure_value.c                       include/structure_value.h
        src/array_value.c                           include/array_value.h
        src/module_value.c                          include/module_value.h
        src/control_flow.c                          include/control_flow.h
        src/parameter.c                             include/parameter.h

        src/utils/execute_file.c                    include/utils/execute_file.h
        src/utils/string_view.c                     include/utils/string_view.h
        src/utils/interpret_info.c                  include/utils/interpret_info.h
        src/utils/memory.c                          include/utils/memory.h
        src/utils/typedefs.c                        include/utils/typedefs.h
        src/utils/throw.c                           include/utils/throw.h

        src/ast/ast_node.c                          include/ast/ast_node.h
        src/ast/binary_op_node.c                    include/ast/binary_op_node.h
        src/ast/literal_node.c                      include/ast/literal_node.h
        src/ast/variable_node.c                     include/ast/variable_node.h
        src/ast/function_call_node.c                include/ast/function_call_node.h
        src/ast/function_definition_node.c          include/ast/function_definition_node.h
        src/ast/structure_definition_node.c         include/ast/structure_definition_node.h
        src/ast/variable_field_access_node.c        include/ast/variable_field_access_node.h
        src/ast/array_definition_node.c             include/ast/array_definition_node.h
        src/ast/array_access_node.c                 include/ast/array_access_node.h
        src/ast/statement_sequence_node.c           include/ast/statement_sequence_node.h
        src/ast/variable_definition_node.c          include/ast/variable_definition_node.h
        src/ast/variable_assignment_node.c          include/ast/variable_assignment_node.h
        src/ast/import_node.c                       include/ast/import_node.h

        src/ast/condition_node.c                    include/ast/condition_node.h
        src/ast/while_loop_node.c                   include/ast/while_loop_node.h
        src/ast/for_loop_node.c                     include/ast/for_loop_node.h
        src/ast/break_node.c                        include/ast/break_node.h
        src/ast/throw_node.c                        include/ast/throw_node.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

add_subdirectory(pesec-std/io)
add_subdirectory(pesec-std/math)
#add_subdirectory(pesec-std/types)
#add_subdirectory(pesec-std/network)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        io
        math
#        types
#        network
)