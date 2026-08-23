#include "include/utils/execute_file.h"

#include <stdio.h>

#include "include/function_value.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/utils/throw.h"

value_t execute_file(const char* filepath, context_t* context)
{
    char* source = nullptr;
    ull_t source_size = 0;

    FILE* file = fopen(filepath, "r");

    if (!file) THROW("Could not open file %s\n", filepath);

    fseek(file, 0, SEEK_END);
    source_size = ftell(file);
    fseek (file, 0, SEEK_SET);
    source = (char*)malloc(source_size);
    fread(source, 1, source_size, file);
    fclose(file);

    lexer_t* lexer = lexer_new(source, source_size);
    parser_t* parser = parser_new(lexer);
    ast_node_t* ast = parser_parse(parser);

    const value_t result = ast ? ast_node_evaluate(ast, context) : MAKE_VAL_NUM(0);

    switch (result.control_flow)
    {
        case CONTROL_FLOW_BREAK: THROW("Break outside of loop\n"); break;
        case CONTROL_FLOW_CONTINUE: THROW("Continue outside of loop\n"); break;
        default: break;
    }

    ast_node_free(ast);
    lexer_free(lexer);
    parser_free(parser);

    return result;
}
