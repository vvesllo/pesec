#include "include/utils/execute_file.h"

#include <stdio.h>

#include "include/function_value.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/structure_value.h"
#include "include/vector_value.h"
#include "include/utils/interpret_info.h"
#include "include/utils/memory.h"
#include "include/utils/throw.h"

static const char* get_platform()
{
#if defined(__linux__)
    return "linux";
#elif defined(_WIN32) || defined(_WIN64)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    return "bsd";
#elif defined(__unix__) || defined(__unix)
    return "unix";
#else
    return "unknown";
#endif
}

static const char* get_architecture()
{
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
    return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return "arm64";
#elif defined(__arm__) || defined(_M_ARM)
    return "arm";
#elif defined(__riscv)
    return "riscv";
#else
    return "unknown";
#endif
}

static void push_constant(context_t* context, const char* name, const value_t value)
{
    context_push(context, string_view_from(name), value, true);
}

static void create_default_values(context_t* context, const char* filepath)
{
    context_t* system_context = context_new(nullptr);
    push_constant(system_context, "platform", value_new_string(string_value_from_cstr(get_platform())));
    push_constant(system_context, "arch", value_new_string(string_value_from_cstr(get_architecture())));
    push_constant(system_context, "name", value_new_string(string_value_from_cstr(filepath)));
    push_constant(system_context, "args", value_new_vector(interpret_info_get()->args));

    push_constant(context, "__system", value_new_structure(structure_value_new(system_context)));
}

value_t execute_file(const char* filepath, context_t* context)
{
    FILE* file = fopen(filepath, "r");

    if (!file) THROW("Could not open file %s\n", filepath);


    fseek(file, 0, SEEK_END);
    const ull_t source_size = ftell(file);
    fseek (file, 0, SEEK_SET);

    const auto source = memory_push(memory_get(), (memory_value_t) {
        .value.as_char = (char*)malloc(source_size),
        .type = MEMORY_VALUE_TYPE_CHAR,
    });

    fread(source->value.as_char, 1, source_size, file);
    fclose(file);

    lexer_t* lexer = lexer_new(source->value.as_char, source_size);
    parser_t* parser = parser_new(lexer);
    ast_node_t* ast = parser_parse(parser);

    create_default_values(context, filepath);

    const value_t result = ast ? ast_node_evaluate(ast, context) : value_new_number(NUM_VAL_0);

    switch (result.control_flow)
    {
        case CONTROL_FLOW_BREAK: THROW("Break outside of loop\n"); break;
        case CONTROL_FLOW_CONTINUE: THROW("Continue outside of loop\n"); break;
        case CONTROL_FLOW_PANIC:
        default: break;
    }

    ast_node_free(ast);
    lexer_free(lexer);
    parser_free(parser);

    return result;
}
