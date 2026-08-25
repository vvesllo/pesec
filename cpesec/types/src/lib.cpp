#include "../include/lib.hpp"

#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"
#include <string>

#include "../../../include/utils/throw.h"

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



static value_t to_string(context_t *context)
{
    const value_t value = context_get(context, string_view_from("value"))->value;

    std::string str;

    switch (value.type)
    {
        case VALUE_TYPE_NUMBER: str = std::to_string(value.data.as_number); break;
            default: THROW("Unknown value type");
    }

    value_t result = {
        .reference_count = 1,
        .type = VALUE_TYPE_STRING
    };

    result.data.as_string = string_value_from_cstr(str.c_str());

    return result;
}

void init_lib(context_t *context)
{
    REG_FUNC(to_string, "value");
}