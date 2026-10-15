#include <stdio.h>

#include "include/common/string_view.h"
#include "include/frontend/lexer.h"
#include "include/frontend/parser.h"

int main()
{
    const pesec_string_view_t source = pesec_string_view_from("(2 + 2) * 2");
    pesec_lexer_t lexer = pesec_lexer_new(source);
    pesec_parser_t parser = pesec_parser_new(&lexer);
    const pesec_node_t* node = pesec_parser_parse(&parser);

    const int64_t result = pesec_node_evaluate(node);

    printf("main::node: %" PRIi64, result);

    pesec_node_destroy(node);

    return 0;
}
