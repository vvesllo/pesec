#include <stdio.h>
#include <stdlib.h>

#include "include/context.h"
#include "include/function_value.h"
#include "include/string_value.h"
#include "include/utils/execute_file.h"
#include "include/utils/interpret_info.h"
#include "include/utils/memory.h"

#define PESEC_MAJOR_VERSION 1
#define PESEC_MINOR_VERSION 1
#define PESEC_PATCH_VERSION 0

static void print_help()
{
    printf(
        "pesec-%d.%d.%d help\n"
        "Usage: pesec [FILE]\n",
        PESEC_MAJOR_VERSION, PESEC_MINOR_VERSION, PESEC_PATCH_VERSION
        );
}

context_t* global_current_context = nullptr;

int main(const int argc, const char** argv)
{
    interpret_info_get()->filename = argv[1];
    interpret_info_get()->number_accuracy = 8;

    vector_value_t* argv_vector = vector_value_new_size(0);

    for (int i = 1; i < argc; i++)
    {
        vector_value_push(
            argv_vector,
            value_new_string(string_value_from_cstr(argv[i]))
        );
    }

    interpret_info_get()->args = argv_vector;

    if (!interpret_info_get()->filename)
    {
        print_help();
        return -1;
    }

    const char* filename = interpret_info_get()->filename;

    global_current_context = context_new(nullptr);
    const value_t result = execute_file(filename, global_current_context);

    if (result.control_flow == CONTROL_FLOW_PANIC)
    {
        const value_t result_string = value_to_string(result);
        fprintf(stderr, "panic: %.*s", (int)result_string.data.as_string->size, result_string.data.as_string->data);
    }

    context_free(global_current_context);
    free(interpret_info_get());
    memory_free(memory_get());

    return EXIT_SUCCESS;
}
