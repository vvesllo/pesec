#include "../include/lib.h"

#include <stdio.h>

#include "../../../include/function_value.h"

static value_t println(context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    value_print(value->value);
    printf("\n");
    return MAKE_VAL_NUM(0);
}

void init_lib(context_t* context)
{
    context_push(
        context,
        string_view_from("println"),
        MAKE_VAL_FUNC(
            function_value_new(
                parameter_new_from_cstr((const char*[]){ "value", nullptr }),
                (function_value_value_t) {
                    .as_c_function = println
                },
                FUNCTION_VALUE_TYPE_C_FUNCTION
            )
        ),
        false
    );
}
