#include "../include/lib.h"

#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"

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

static value_t ahoy(context_t *context)
{
    return value_new_string(string_value_from_cstr("Ahoy!"));
}

void init_lib(context_t *context)
{
    REG_FUNC(ahoy, nullptr);
}