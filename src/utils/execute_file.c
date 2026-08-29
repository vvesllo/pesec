#include "include/utils/execute_file.h"

#include <stdio.h>

#include "include/function_value.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/utils/throw.h"

static const char* get_platform()
{
#ifdef __linux__
    return "linux";
#elifdef _WIN32 || _WIN64
    return "windows"
#else
    return "unknown";
#endif
}

static void create_default_values(context_t* context)
{
    context_push(
        context,
        string_view_from("__platform"),
        value_new_string(string_value_from_cstr(get_platform())),
        true);
}

value_t execute_file(const char* filepath, context_t* context)
{
    FILE* file = fopen(filepath, "r");

    if (!file) THROW("Could not open file %s\n", filepath);

    fseek(file, 0, SEEK_END);
    const ull_t source_size = ftell(file);
    fseek (file, 0, SEEK_SET);
    const auto source = (char*)malloc(source_size);
    fread(source, 1, source_size, file);
    fclose(file);

    lexer_t* lexer = lexer_new(source, source_size);
    parser_t* parser = parser_new(lexer);
    ast_node_t* ast = parser_parse(parser);

    create_default_values(context);

    const value_t result = ast ? ast_node_evaluate(ast, context) : value_new_number(NUM_VAL_0);

    switch (result.control_flow)
    {
        case CONTROL_FLOW_BREAK: THROW("Break outside of loop\n"); break;
        case CONTROL_FLOW_CONTINUE: THROW("Continue outside of loop\n"); break;
        case CONTROL_FLOW_THROW:
        default: break;
    }

    ast_node_free(ast);
    lexer_free(lexer);
    parser_free(parser);

    return result;
}
