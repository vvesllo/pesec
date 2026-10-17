#include <stdio.h>

#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"
#include "include/runtime/variable_table.h"


pesec_value_t print_function(size_t argc, pesec_value_t* argv, pesec_environment_t* environment)
{
    for (size_t i = 0; i < argc; i++)
    {
        if (i > 0) printf(", ");
        printf("%" PRIi64, argv->data.integer);
    }
    printf("\n");
    return pesec_value_i64(0);
}

int main()
{
    const pesec_string_view_t source = pesec_string_view_from("mut test = 23; immut a = 3; print(420); test * a;");
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

    const pesec_value_t result = pesec_node_evaluate(node, &global_environment);
    printf("main::node: %" PRIi64, result.data.integer);

    pesec_node_destroy(node);

    return 0;
}
