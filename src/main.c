#include <stdio.h>

#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"
#include "include/runtime/variable_table.h"

int main()
{
    pesec_variable_table_t variable_table = pesec_variable_table_new();
    pesec_variable_table_push(&variable_table, (pesec_variable_t){
        .name = pesec_string_view_from("test"),
        .value = 69'420,
        .immutable = true
    });


    const pesec_variable_t* var = pesec_variable_table_get(&variable_table, pesec_string_view_from("test"));

    printf("test{value=%" PRIi64 ", immutable=%s}\n", var->value, var->immutable ? "true" : "false");

    pesec_variable_table_destroy(&variable_table);

    return 0;

    const pesec_string_view_t source = pesec_string_view_from("(2 + 2) * 2");
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    const pesec_node_t* node = pesec_parser_parse(&parser);

    const pesec_value_t result = pesec_node_evaluate(node);

    printf("main::node: %" PRIi64, result);

    pesec_node_destroy(node);

    return 0;
}
