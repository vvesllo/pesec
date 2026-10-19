#include <stdio.h>

#include "include/common/alloc.h"
#include "include/common/file_read.h"
#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"

#include "include/runtime/environment.h"


static pesec_value_t print_function(pesec_environment_t* environment)
{
    const pesec_variable_t* printable = pesec_environment_get_variable(
        environment,
        pesec_string_view_from("value")
    );
    pesec_value_print(stdout, printable->value);
    printf("\n");
    return pesec_value_i64(0);
}
/*
 * TODO: optional, value.control_flow, while loop, loop labels etc
 */
int main(int argc, char** argv)
{
    const pesec_string_view_t source = pesec_file_read(argv[1]);
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    const pesec_node_t* node = pesec_parser_parse(&parser);

    pesec_environment_t* global_environment = pesec_environment_new(nullptr);

    pesec_environment_push_variable(
        global_environment,
        pesec_string_view_from("print"),
        pesec_value_function((pesec_function_value_t){
            .type = PESEC_FUNCTION_VALUE_TYPE_NATIVE,
            .data.native = print_function,
            .parameter_vector = pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            .parent_environment = global_environment
        }),
        true
    );

    pesec_node_evaluate(node, global_environment);
    pesec_node_destroy(node);
    pesec_alloc_cleanup_all();

    return 0;
}
