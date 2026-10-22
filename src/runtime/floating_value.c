#include "include/runtime/floating_value.h"

#include <math.h>

#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/integer_value.h"

pesec_floating_value_t* pesec_floating_value_new(const double value)
{
    const auto floating_value = (pesec_floating_value_t*)pesec_malloc(sizeof(pesec_floating_value_t));

    floating_value->value = value;

    return floating_value;
}

void pesec_floating_value_destroy(pesec_floating_value_t* floating_value)
{
    pesec_free(floating_value);
}

void pesec_floating_value_print(FILE* stream, const pesec_floating_value_t* floating_value)
{
    fprintf(stream, "%g", floating_value->value);
}

double pesec_floating_value_get(const pesec_floating_value_t* floating_value)
{
    return floating_value->value;
}

bool pesec_floating_value_is_truthy(const pesec_floating_value_t* floating_value)
{
    return floating_value->value != 0.0;
}

pesec_value_t pesec_floating_value_negative(const pesec_floating_value_t* floating_value)
{
    return pesec_value_as_floating(pesec_floating_value_new(-floating_value->value));
}

pesec_value_t pesec_floating_value_add(const pesec_floating_value_t* left, const pesec_value_t right)
{
    pesec_floating_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_floating_value_new(left->value + right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_floating_value_new(left->value + right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("floating_value::add: unsupported right type");
    return pesec_value_as_floating(result);
}

pesec_value_t pesec_floating_value_sub(const pesec_floating_value_t* left, const pesec_value_t right)
{
    pesec_floating_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_floating_value_new(left->value - right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_floating_value_new(left->value - right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("floating_value::sub: unsupported right type");
    return pesec_value_as_floating(result);
}

pesec_value_t pesec_floating_value_mul(const pesec_floating_value_t* left, const pesec_value_t right)
{
    pesec_floating_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_floating_value_new(left->value * right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_floating_value_new(left->value * right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("floating_value::mul: unsupported right type");
    return pesec_value_as_floating(result);
}

pesec_value_t pesec_floating_value_div(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            if (right.data.floating->value == 0.0)
                pesec_panic("floating_value::div: division by zero(0)");
            return pesec_value_as_floating(
                pesec_floating_value_new(left->value / right.data.floating->value)
            );

        case PESEC_VALUE_TYPE_INTEGER:
            if (right.data.integer->value == 0)
                pesec_panic("floating_value::div: division by zero(0)");
            return pesec_value_as_floating(
                pesec_floating_value_new(left->value / (double)right.data.integer->value)
            );

        default:
            pesec_panic("floating_value::div: unsupported right type");
    }
}

pesec_value_t pesec_floating_value_mod(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_floating(
                pesec_floating_value_new(fmod(left->value, right.data.floating->value))
            );

        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_floating(
                pesec_floating_value_new(fmod(left->value, (double)right.data.integer->value))
            );

        default:
            pesec_panic("floating_value::mod: unsupported right type");
    }
}

bool pesec_floating_value_equal(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING: return left->value == right.data.floating->value;
        case PESEC_VALUE_TYPE_INTEGER: return left->value == right.data.integer->value;
        default: pesec_panic("floating_value::equal: unsupported right type");
    }
}

bool pesec_floating_value_less(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING: return left->value < right.data.floating->value;
        case PESEC_VALUE_TYPE_INTEGER: return left->value < right.data.integer->value;
        default: pesec_panic("floating_value::less: unsupported right type");
    }
}

bool pesec_floating_value_less_equal(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING: return left->value <= right.data.floating->value;
        case PESEC_VALUE_TYPE_INTEGER: return left->value <= right.data.integer->value;
        default: pesec_panic("floating_value::less_equal: unsupported right type");
    }
}

bool pesec_floating_value_greater(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING: return left->value > right.data.floating->value;
        case PESEC_VALUE_TYPE_INTEGER: return left->value > right.data.integer->value;
        default: pesec_panic("floating_value::greater: unsupported right type");
    }
}

bool pesec_floating_value_greater_equal(const pesec_floating_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING: return left->value >= right.data.floating->value;
        case PESEC_VALUE_TYPE_INTEGER: return left->value >= right.data.integer->value;
        default: pesec_panic("floating_value::greater_equal: unsupported right type");
    }
}

pesec_value_t pesec_floating_value_method_handler(
    pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}
