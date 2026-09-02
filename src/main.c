#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/context.h"
#include "include/function_value.h"
#include "include/utils/execute_file.h"
#include "include/utils/interpret_info.h"
#include "include/utils/memory.h"
#include "include/utils/throw.h"

#define PESEC_MAJOR_VERSION 1
#define PESEC_MINOR_VERSION 0
#define PESEC_PATCH_VERSION 0

static bool is_tag(const char* arg)
{
    return strlen(arg) > 2 &&
        arg[0] == '-' && arg[1] == '-';
}

static bool arg_equals(const char* arg, const char* value)
{
    return strcmp(arg, value) == 0;
}

static void print_help()
{
    printf(
        "pesec-%d.%d.%d help\n"
        "\t--file [FILE] - run file\n"
        "\t--nacc [SIZE] - sets number fraction part accuracy\n",
        PESEC_MAJOR_VERSION, PESEC_MINOR_VERSION, PESEC_PATCH_VERSION
        );
}

static void process_args(const int argc, const char** argv)
{
    unsigned int i = 1;

    while (i < argc)
    {
        if (is_tag(argv[i]))
        {
            if (arg_equals(argv[i], "--help")) print_help();
            else if (arg_equals(argv[i], "--file"))
            {
                if (++i >= argc) THROW("Missing argument for --file\n");
                interpret_info_get()->filename = argv[i];
            }
            else if (arg_equals(argv[i], "--nacc"))
            {
                if (++i >= argc) THROW("Missing argument for --nacc\n");
                interpret_info_get()->number_accuracy = strtoull(argv[i], nullptr, 10);
            }
        }
        else THROW("Unknown tag: %s, use 'pesec --help'\n", argv[i]);
        i++;
    }
}


int main(const int argc, const char** argv)
{
    interpret_info_get()->number_accuracy = 8;

    process_args(argc, argv);

    if (!interpret_info_get()->filename)
    {
        print_help();
        return -1;
    }

    const char* filename = interpret_info_get()->filename;

    context_t* context = context_new(nullptr);
    const value_t result = execute_file(filename, context);
    if (result.control_flow == CONTROL_FLOW_THROW)
        value_print(stderr, result);

    context_free(context);

    free(interpret_info_get());

    memory_free(memory_get());

    return EXIT_SUCCESS;
}
