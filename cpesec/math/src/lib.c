#include "../include/lib.h"

#include <math.h>

#include "../../../include/function_value.h"
#include "../../../include/number_value.h"
#include "../../../include/value.h"
#include "include/utils/throw.h"


#define REG_FUNC(function_name, ...) \
    context_push( \
        context, \
        string_view_from(#function_name), \
        value_new_function( \
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
    const value_t value = context_get(context, string_view_from("value"))->value;
    if (value.type == VALUE_TYPE_NUMBER)
    {
        number_value_t* num = value.data.as_number;
        const long double x = number_value_to_long_double(num);
        const long double result = sinl(x);
        number_value_t* res_num = number_value_from_long_double(result);
        return value_new_number(res_num);
    }
    THROW("Value should be a number");
}

static value_t _cos(context_t* context)
{
    const value_t value = context_get(context, string_view_from("value"))->value;
    if (value.type == VALUE_TYPE_NUMBER)
    {
        number_value_t* num = value.data.as_number;
        const long double x = number_value_to_long_double(num);
        const long double result = cosl(x);
        number_value_t* res_num = number_value_from_long_double(result);
        return value_new_number(res_num);
    }
    THROW("Value should be a number");
}

void init_lib(context_t *context)
{
    REG_FUNC(_sin, "value");
    REG_FUNC(_cos, "value");
}