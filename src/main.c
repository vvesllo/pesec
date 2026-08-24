#include <stdio.h>
#include <stdlib.h>

#include "include/context.h"
#include "include/function_value.h"
#include "include/utils/execute_file.h"

int main(const int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: pesec <file>\n");
        return EXIT_SUCCESS;
    }

    const char* filename = argv[1];

    context_t* context = context_new(nullptr);
    const value_t result = execute_file(filename, context);
    if (result.control_flow == CONTROL_FLOW_THROW)
        value_print(stderr, result);

    context_free(context);

    return EXIT_SUCCESS;
}
