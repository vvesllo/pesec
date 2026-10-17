#include <stdio.h>

#include "include/common/alloc.h"
#include "include/common/file_read.h"
#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"
#include "include/runtime/variable_table.h"


static pesec_value_t print_function(const size_t argc, pesec_value_t* argv, pesec_environment_t* environment)
{
    for (size_t i = 0; i < argc; i++)
    {
        if (i > 0) printf(", ");
        printf("%" PRIi64, argv[i].data.integer);
    }
    printf("\n");
    return pesec_value_i64(0);
}

int main(int argc, char** argv)
{
    const pesec_string_view_t source = pesec_file_read(argv[1]);
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    const pesec_node_t* node = pesec_parser_parse(&parser);

    pesec_environment_t global_environment = pesec_environment_new(nullptr);

    pesec_environment_push_variable(
        &global_environment,
        pesec_string_view_from("print"),
        pesec_value_function(print_function),
        true
    );

    pesec_node_evaluate(node, &global_environment);
    pesec_node_destroy(node);
    pesec_alloc_cleanup_all();

    return 0;
}
