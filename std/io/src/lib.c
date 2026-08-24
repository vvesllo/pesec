#include "../include/lib.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../../include/function_value.h"

#define REG_FUNC(function_name, ...) \
    context_push(\
        context,\
        string_view_from(#function_name),\
        MAKE_VAL_FUNC(\
            function_value_new(\
                parameter_new_from_cstr((const char *[]){__VA_ARGS__, nullptr}),\
                (function_value_value_t) {\
                    .as_c_function = function_name\
                },\
                FUNCTION_VALUE_TYPE_C_FUNCTION\
            )\
        ), true)

static value_t cout(context_t *context)
{
    const context_item_t *value = context_get(context, string_view_from("value"));
    value_print(value->value);
    return MAKE_VAL_NUM(0);
}

static value_t cin(context_t *context)
{
    char *line;
    size_t capacity = 0;

    const ssize_t length = getline(&line, &capacity, stdin);

    if (length != -1)
    {
        if (length > 0 && line[length - 1] == '\n')
            line[length - 1] = '\0';
    }

    return MAKE_VAL_STR(
        string_value_from(line, length)
    );
}

void init_lib(context_t *context)
{
    REG_FUNC(cout, "value");
    REG_FUNC(cin, nullptr);
}
