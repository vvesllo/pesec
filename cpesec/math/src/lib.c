#include "../include/lib.h"

#include <math.h>

#include "../../../include/function_value.h"
#include "../../../include/value.h"
#include "include/utils/throw.h"

#define REG_FUNC(function_name, ...) \
    context_push( \
        context, \
        string_view_from(#function_name), \
        MAKE_VAL_FUNC( \
            function_value_new( \
                parameter_new_from_cstr((const char *[]){__VA_ARGS__, nullptr}), \
                (function_value_value_t){ \
                    .as_c_function = function_name \
                }, \
                FUNCTION_VALUE_TYPE_C_FUNCTION, \
                context \
            ) \
        ), \
        true \
    )

static value_t _sin(context_t* context)
{
    value_t value = context_get(context, string_view_from("value"))->value;
    if (value.type == VALUE_TYPE_NUMBER)
    {
        return MAKE_VAL_NUM(sin(value.data.as_number));
    }

    THROW("fuck.");
}

void init_lib(context_t *context)
{
    REG_FUNC(_sin, "value");
}