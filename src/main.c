#include <stdio.h>

#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"
#include "include/runtime/variable_table.h"

int main()
{
    const pesec_string_view_t source = pesec_string_view_from("add(69 * 1000, add(210, 210))");
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    const pesec_node_t* node = pesec_parser_parse(&parser);

    pesec_environment_t environment = pesec_environment_new(nullptr);
    pesec_variable_table_push(&environment.variable_table, (pesec_variable_t){
        .name = pesec_string_view_from("test"),
        .value = 69'420,
        .immutable = true
    });

    const pesec_value_t result = pesec_node_evaluate(node, &environment);

    printf("main::node: %" PRIi64, result.data.integer);

    pesec_node_destroy(node);

    return 0;
}
