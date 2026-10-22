#include "include/runtime/integer_value.h"

#include <math.h>

#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/floating_value.h"

pesec_integer_value_t* pesec_integer_value_new(const int64_t value)
{
    const auto integer_value = (pesec_integer_value_t*)pesec_malloc(sizeof(pesec_integer_value_t));

    integer_value->value = value;

    return integer_value;
}

void pesec_integer_value_destroy(pesec_integer_value_t* integer_value)
{
    pesec_free(integer_value);
}

void pesec_integer_value_print(FILE* stream, const pesec_integer_value_t* integer_value)
{
    fprintf(stream, "%" PRIi64, integer_value->value);
}

int64_t pesec_integer_value_get(const pesec_integer_value_t* integer_value)
{
    return integer_value->value;
}

bool pesec_integer_value_is_truthy(const pesec_integer_value_t* integer_value)
{
    return integer_value->value != 0;
}

pesec_value_t pesec_integer_value_negative(const pesec_integer_value_t* integer_value)
{
    return pesec_value_as_integer(pesec_integer_value_new(-integer_value->value));
}

pesec_value_t pesec_integer_value_add(const pesec_integer_value_t* left, const pesec_value_t right)
{
    pesec_integer_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_integer_value_new(left->value + right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_integer_value_new(left->value + right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("integer_value::add: unsupported right type");
    return pesec_value_as_integer(result);
}

pesec_value_t pesec_integer_value_sub(const pesec_integer_value_t* left, const pesec_value_t right)
{
    pesec_integer_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_integer_value_new(left->value - right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_integer_value_new(left->value - right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("integer_value::sub: unsupported right type");
    return pesec_value_as_integer(result);
}

pesec_value_t pesec_integer_value_mul(const pesec_integer_value_t* left, const pesec_value_t right)
{
    pesec_integer_value_t* result = nullptr;
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            result = pesec_integer_value_new(left->value * right.data.floating->value);
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            result = pesec_integer_value_new(left->value * right.data.integer->value);
            break;
        default:
            break;
    }

    if (!result) pesec_panic("integer_value::mul: unsupported right type");
    return pesec_value_as_integer(result);
}

pesec_value_t pesec_integer_value_div(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            if (right.data.floating->value == 0.0)
                pesec_panic("integer_value::div: division by zero(0)");
            return pesec_value_as_floating(
                pesec_floating_value_new((double)left->value / right.data.floating->value)
            );

        case PESEC_VALUE_TYPE_INTEGER:
            if (right.data.integer->value == 0)
                pesec_panic("integer_value::div: division by zero(0)");
            return pesec_value_as_integer(
                pesec_integer_value_new(left->value / right.data.integer->value)
            );

        default:
            pesec_panic("integer_value::div: unsupported right type");
    }
}

pesec_value_t pesec_integer_value_mod(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_FLOATING:
            if (right.data.floating->value == 0.0)
                pesec_panic("integer_value::mod: division by zero(0)");
            return pesec_value_as_floating(
                pesec_floating_value_new(fmod((double)left->value, right.data.floating->value))
            );

        case PESEC_VALUE_TYPE_INTEGER:
            if (right.data.integer->value == 0)
                pesec_panic("integer_value::mod: division by zero(0)");
            return pesec_value_as_integer(
                pesec_integer_value_new(left->value % right.data.integer->value)
            );

        default:
            pesec_panic("integer_value::mod: unsupported right type");
    }
}

bool pesec_integer_value_equal(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return left->value == right.data.integer->value;
        case PESEC_VALUE_TYPE_FLOATING:
            return (double)left->value == right.data.floating->value;
        default:
            pesec_panic("integer_value::equal: unsupported right type");
    }
}

bool pesec_integer_value_less(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return left->value < right.data.integer->value;
        case PESEC_VALUE_TYPE_FLOATING:
            return (double)left->value < right.data.floating->value;
        default:
            pesec_panic("integer_value::less: unsupported right type");
    }
}

bool pesec_integer_value_less_equal(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return left->value <= right.data.integer->value;
        case PESEC_VALUE_TYPE_FLOATING:
            return (double)left->value <= right.data.floating->value;
        default:
            pesec_panic("integer_value::less_equal: unsupported right type");
    }
}

bool pesec_integer_value_greater(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return left->value > right.data.integer->value;
        case PESEC_VALUE_TYPE_FLOATING:
            return (double)left->value > right.data.floating->value;
        default:
            pesec_panic("integer_value::greater: unsupported right type");
    }
}

bool pesec_integer_value_greater_equal(const pesec_integer_value_t* left, const pesec_value_t right)
{
    switch (right.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return left->value >= right.data.integer->value;
        case PESEC_VALUE_TYPE_FLOATING:
            return (double)left->value >= right.data.floating->value;
        default:
            pesec_panic("integer_value::greater_equal: unsupported right type");
    }
}

pesec_value_t pesec_integer_value_method_handler(
    pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}
