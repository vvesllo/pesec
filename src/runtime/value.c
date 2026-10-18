#include "include/runtime/value.h"

#include "include/common/panic.h"


static pesec_value_t pesec_value_new(const pesec_value_type_t type, const pesec_value_data_t data)
{
    return (pesec_value_t){
        .type = type,
        .data = data
    };
}

pesec_value_t pesec_value_i64(const int64_t data)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_I64,
        (pesec_value_data_t){
            .integer = data
        }
    );
}

pesec_value_t pesec_value_function(const pesec_function_value_t data)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_FUNCTION,
        (pesec_value_data_t){
            .function = data
        }
    );
}

bool pesec_value_is_truthy(const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_I64: return value.data.integer != 0;
        case PESEC_VALUE_TYPE_FUNCTION: return true;
    }

    pesec_panic("value::is_truthy: unknown value type (%d)", value.type);
}

pesec_value_t pesec_value_add(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::add: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer + right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::add: fuck");
}

pesec_value_t pesec_value_sub(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::sub: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer - right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::sub: fuck");
}

pesec_value_t pesec_value_mul(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::mul: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer * right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::mul: fuck");
}

pesec_value_t pesec_value_div(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::div: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer / right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::div: fuck");
}

pesec_value_t pesec_value_equal(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::equal: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer == right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::equal: fuck");
}

pesec_value_t pesec_value_less(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::less: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer < right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::less: fuck");
}

pesec_value_t pesec_value_less_equal(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::less_equal: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer <= right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::less_equal: fuck");
}

pesec_value_t pesec_value_greater(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::greater: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer > right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::greater: fuck");
}

pesec_value_t pesec_value_greater_equal(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::greater_equal: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_i64(left.data.integer >= right.data.integer);
        case PESEC_VALUE_TYPE_FUNCTION:
            return pesec_value_i64(0);
    }
    pesec_panic("value::greater_equal: fuck");
}
