#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/common/alloc.h"
#include "include/common/file_read.h"
#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"

#include "include/runtime/environment.h"
#include "include/runtime/function_value.h"
#include "include/runtime/integer_value.h"


static pesec_value_t print_function(pesec_environment_t* environment)
{
    const pesec_variable_t* printable = pesec_environment_get_variable(
        environment,
        pesec_string_view_from("value")
    );
    pesec_value_print(stdout, printable->value);
    return pesec_value_as_integer(pesec_integer_value_new(0));
}

static pesec_value_t println_function(pesec_environment_t* environment)
{
    print_function(environment);
    printf("\n");
    return pesec_value_as_integer(pesec_integer_value_new(0));
}

static void pesec_print_help()
{
    printf("help\n");
}

int main(const int argc, char** argv)
{
    const clock_t start = clock();
    if (argc < 2)
    {
        pesec_print_help();
        return EXIT_FAILURE;
    }

    const pesec_string_view_t source = pesec_file_read(argv[1]);
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    pesec_node_t* node = pesec_parser_parse(&parser);

    pesec_environment_t* global_environment = pesec_environment_new(nullptr);

    pesec_environment_push_variable(
        global_environment,
        pesec_string_view_from("print"),
        pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_NATIVE,
            (pesec_function_value_data_t){.native = print_function},
            pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            global_environment
        )),
        true
    );

    pesec_environment_push_variable(
        global_environment,
        pesec_string_view_from("println"),
        pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_NATIVE,
            (pesec_function_value_data_t){.native = println_function},
            pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            global_environment
        )),
        true
    );

    pesec_node_evaluate(node, global_environment);

    const clock_t stop = clock();
    const double elapsed = (double)(stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f", elapsed);

    pesec_node_destroy(node);
    pesec_alloc_cleanup_all();
    return 0;
}
