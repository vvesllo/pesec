#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/context.h"
#include "include/function_value.h"
#include "include/utils/execute_file.h"
#include "include/utils/throw.h"


static struct
{
    ull_t number_accurate;
    const char* filename;
} interpret_info;

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
        "Pesec help\n"
        "\t--file [FILE] - run file\n"
        "\t--nacc [SIZE] - sets number fraction part accuracy\n"
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
                interpret_info.filename = argv[i];
            }
            else if (arg_equals(argv[i], "--nacc"))
            {
                if (++i >= argc) THROW("Missing argument for --nacc\n");
                interpret_info.number_accurate = strtoull(argv[i], nullptr, 10);
            }
        }
        else THROW("Unknown tag: `%s`\n", argv[i]);
        i++;
    }
}


int main(const int argc, const char** argv)
{
    interpret_info.number_accurate = 16;

    process_args(argc, argv);

    if (!interpret_info.filename)
    {
        print_help();
        return -1;
    }

    const char* filename = interpret_info.filename;

    context_t* context = context_new(nullptr);
    const value_t result = execute_file(filename, context);
    if (result.control_flow == CONTROL_FLOW_THROW)
        value_print(stderr, result);

    context_free(context);

    return EXIT_SUCCESS;
}
