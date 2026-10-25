#include "include/runtime/number_value.h"

#include <math.h>

#include "include/common/alloc.h"
#include "include/common/panic.h"

pesec_number_value_t* pesec_number_value_new(const double value)
{
    const auto number_value = (pesec_number_value_t*)pesec_malloc(sizeof(pesec_number_value_t));

    number_value->value = value;

    return number_value;
}

void pesec_number_value_destroy(pesec_number_value_t* number_value)
{
    pesec_free(number_value);
}

void pesec_number_value_print(FILE* stream, const pesec_number_value_t* number_value)
{
    fprintf(stream, "%g", number_value->value);
}

double pesec_number_value_get(const pesec_number_value_t* number_value)
{
    return number_value->value;
}

bool pesec_number_value_is_truthy(const pesec_number_value_t* number_value)
{
    return number_value->value != 0.0;
}

pesec_value_t pesec_number_value_negative(const pesec_number_value_t* number_value)
{
    return pesec_value_as_number(pesec_number_value_new(-number_value->value));
}

pesec_value_t pesec_number_value_add(const pesec_number_value_t* left, const pesec_value_t right)
{
    pesec_number_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER:
            result = pesec_number_value_new(left->value + right.data.number->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("number_value::add: unsupported right type");
    return pesec_value_as_number(result);
}

pesec_value_t pesec_number_value_sub(const pesec_number_value_t* left, const pesec_value_t right)
{
    pesec_number_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER:
            result = pesec_number_value_new(left->value - right.data.number->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("number_value::sub: unsupported right type");
    return pesec_value_as_number(result);
}

pesec_value_t pesec_number_value_mul(const pesec_number_value_t* left, const pesec_value_t right)
{
    pesec_number_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER:
            result = pesec_number_value_new(left->value * right.data.number->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("number_value::mul: unsupported right type");
    return pesec_value_as_number(result);
}

pesec_value_t pesec_number_value_div(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER:
            if (right.data.number->value == 0.0)
                pesec_panic("number_value::div: division by zero(0)");
            return pesec_value_as_number(
                pesec_number_value_new(left->value / right.data.number->value)
            );

        default:
            pesec_panic("number_value::div: unsupported right type");
    }
}

pesec_value_t pesec_number_value_mod(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER:
            return pesec_value_as_number(
                pesec_number_value_new(fmod(left->value, right.data.number->value))
            );

        default:
            pesec_panic("number_value::mod: unsupported right type");
    }
}

bool pesec_number_value_equal(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER: return left->value == right.data.number->value;
        default: pesec_panic("number_value::equal: unsupported right type");
    }
}

bool pesec_number_value_less(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER: return left->value < right.data.number->value;
        default: pesec_panic("number_value::less: unsupported right type");
    }
}

bool pesec_number_value_less_equal(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER: return left->value <= right.data.number->value;
        default: pesec_panic("number_value::less_equal: unsupported right type");
    }
}

bool pesec_number_value_greater(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER: return left->value > right.data.number->value;
        default: pesec_panic("number_value::greater: unsupported right type");
    }
}

bool pesec_number_value_greater_equal(const pesec_number_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_NUMBER: return left->value >= right.data.number->value;
        default: pesec_panic("number_value::greater_equal: unsupported right type");
    }
}

pesec_value_t pesec_number_value_method_handler(
    pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}