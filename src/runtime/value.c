#include "include/runtime/value.h"

#include <math.h>

#include "include/common/panic.h"
#include "include/runtime/integer_value.h"
#include "include/runtime/floating_value.h"
#include "include/runtime/function_value.h"
#include "include/runtime/object_value.h"
#include "include/runtime/string_value.h"
#include "include/runtime/vector_value.h"

static pesec_value_t pesec_value_new(
    const pesec_value_type_t type,
    const pesec_value_data_t data,
    const pesec_value_control_flow_t control_flow
)
{
    return (pesec_value_t){
        .type = type,
        .data = data,
        .control_flow = control_flow,
        .reference_count = 1
    };
}

static void pesec_value_destroy(const pesec_value_t* value)
{
    switch (value->type)
    {
        case PESEC_VALUE_TYPE_NONE:
        case PESEC_VALUE_TYPE_BOOLEAN:
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            pesec_integer_value_destroy(value->data.integer);
            break;
        case PESEC_VALUE_TYPE_FLOATING:
            pesec_floating_value_destroy(value->data.floating);
            break;
        case PESEC_VALUE_TYPE_FUNCTION:
            pesec_function_value_destroy(value->data.function);
            break;
        case PESEC_VALUE_TYPE_VECTOR:
            pesec_vector_value_destroy(value->data.vector);
            break;
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_object_value_destroy(value->data.object);
            break;
        case PESEC_VALUE_TYPE_STRING:
            pesec_string_value_destroy(value->data.string);
            break;
    }
}

void pesec_value_print(FILE* stream, const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_NONE:
            fprintf(stream, "<none>");
            break;
        case PESEC_VALUE_TYPE_BOOLEAN:
            fprintf(stream, value.data.boolean ? "true" : "false");
            break;
        case PESEC_VALUE_TYPE_INTEGER:
            pesec_integer_value_print(stream, value.data.integer);
            break;
        case PESEC_VALUE_TYPE_FLOATING:
            pesec_floating_value_print(stream, value.data.floating);
            break;
        case PESEC_VALUE_TYPE_FUNCTION:
            pesec_function_value_print(stream, value.data.function);
            break;
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_object_value_print(stream, value.data.object);
            break;
        case PESEC_VALUE_TYPE_STRING:
            pesec_string_value_print(stream, value.data.string);
            break;
        case PESEC_VALUE_TYPE_VECTOR: {
            pesec_vector_value_print(stream, value.data.vector);
            break;
        }
    }
}

pesec_value_t pesec_value_as_boolean(const bool data)
{
    return pesec_value_as_boolean_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_integer(pesec_integer_value_t* data)
{
    return pesec_value_as_integer_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_floating(pesec_floating_value_t* data)
{
    return pesec_value_as_floating_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_function(pesec_function_value_t* data)
{
    return pesec_value_as_function_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_vector(pesec_vector_value_t* data)
{
    return pesec_value_as_vector_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_object(pesec_object_value_t* data)
{
    return pesec_value_as_object_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_string(pesec_string_value_t* data)
{
    return pesec_value_as_string_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_none()
{
    return pesec_value_as_none_with(PESEC_VALUE_CONTROL_FLOW_NONE);
}


pesec_value_t pesec_value_as_boolean_with(const bool data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_BOOLEAN,
        (pesec_value_data_t){
            .boolean = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_integer_with(pesec_integer_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_INTEGER,
        (pesec_value_data_t){
            .integer = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_floating_with(pesec_floating_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_FLOATING,
        (pesec_value_data_t){
            .floating = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_function_with(pesec_function_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_FUNCTION,
        (pesec_value_data_t){
            .function = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_vector_with(pesec_vector_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_VECTOR,
        (pesec_value_data_t){
            .vector = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_object_with(pesec_object_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_OBJECT,
        (pesec_value_data_t){
            .object = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_string_with(pesec_string_value_t* data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_STRING,
        (pesec_value_data_t){
            .string = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_none_with(const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_NONE,
        (pesec_value_data_t){0},
        control_flow
    );
}

void pesec_value_increase_reference(pesec_value_t* value)
{
    value->reference_count += 1;
}

void pesec_value_decrease_reference(pesec_value_t* value)
{
    value->reference_count -= 1;
    if (value->reference_count <= 0)
        pesec_value_destroy(value);
}

bool pesec_value_is_truthy(const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_NONE: return false;
        case PESEC_VALUE_TYPE_BOOLEAN: return value.data.boolean;
        case PESEC_VALUE_TYPE_INTEGER: return pesec_integer_value_is_truthy(value.data.integer);
        case PESEC_VALUE_TYPE_FLOATING: return pesec_floating_value_is_truthy(value.data.floating);
        case PESEC_VALUE_TYPE_VECTOR: return pesec_vector_value_is_truthy(value.data.vector);
        case PESEC_VALUE_TYPE_STRING: return pesec_string_value_is_truthy(value.data.string);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT: return true;
    }

    pesec_panic("value::is_truthy: unknown value type (%d)", value.type);
}

pesec_value_t pesec_value_negative(const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_negative(value.data.integer);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_negative(value.data.floating);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_negative(value.data.vector);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_negative(value.data.string);
        case PESEC_VALUE_TYPE_NONE:
            pesec_panic("value::negative: none value can't be negatived");
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::negative: unsupported value type");
    }

    pesec_panic("value::negative: fuck");
}

pesec_value_t pesec_value_add(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_add(left.data.integer, right);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_add(left.data.floating, right);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_add(left.data.vector, right);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_add(left.data.string, right);
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::add: unsupported value type");
    }

    pesec_panic("value::add: fuck");
}

pesec_value_t pesec_value_sub(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_sub(left.data.integer, right);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_sub(left.data.floating, right);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_sub(left.data.vector, right);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_sub(left.data.string, right);
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::sub: unsupported value type");
    }

    pesec_panic("value::sub: fuck");
}

pesec_value_t pesec_value_mul(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_mul(left.data.integer, right);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_mul(left.data.floating, right);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_mul(left.data.vector, right);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_mul(left.data.string, right);
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::mul: unsupported value type");
    }

    pesec_panic("value::mul: fuck");
}

pesec_value_t pesec_value_div(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_div(left.data.integer, right);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_div(left.data.floating, right);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_div(left.data.vector, right);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_div(left.data.string, right);
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::div: unsupported value type");
    }

    pesec_panic("value::div: fuck");
}

pesec_value_t pesec_value_mod(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_integer_value_mod(left.data.integer, right);
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_floating_value_mod(left.data.floating, right);
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_mod(left.data.vector, right);
        case PESEC_VALUE_TYPE_STRING:
            return pesec_string_value_mod(left.data.string, right);
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::mod: unsupported value type");
    }

    pesec_panic("value::mod: fuck");
}

pesec_value_t pesec_value_equal(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_boolean(pesec_integer_value_equal(left.data.integer, right));
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_boolean(pesec_floating_value_equal(left.data.floating, right));
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_boolean(pesec_vector_value_equal(left.data.vector, right));
        case PESEC_VALUE_TYPE_STRING:
            return pesec_value_as_boolean(pesec_string_value_equal(left.data.string, right));
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::equal: unsupported value type");
    }

    pesec_panic("value::equal: fuck");
}

pesec_value_t pesec_value_less(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_boolean(pesec_integer_value_less(left.data.integer, right));
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_boolean(pesec_floating_value_less(left.data.floating, right));
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_boolean(pesec_vector_value_less(left.data.vector, right));
        case PESEC_VALUE_TYPE_STRING:
            return pesec_value_as_boolean(pesec_string_value_less(left.data.string, right));
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::less: unsupported value type");
    }

    pesec_panic("value::less: fuck");
}

pesec_value_t pesec_value_less_equal(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_boolean(pesec_integer_value_less_equal(left.data.integer, right));
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_boolean(pesec_floating_value_less_equal(left.data.floating, right));
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_boolean(pesec_vector_value_less_equal(left.data.vector, right));
        case PESEC_VALUE_TYPE_STRING:
            return pesec_value_as_boolean(pesec_string_value_less_equal(left.data.string, right));
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::less_equal: unsupported value type");
    }

    pesec_panic("value::less_equal: fuck");
}

pesec_value_t pesec_value_greater(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_boolean(pesec_integer_value_greater(left.data.integer, right));
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_boolean(pesec_floating_value_greater(left.data.floating, right));
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_boolean(pesec_vector_value_greater(left.data.vector, right));
        case PESEC_VALUE_TYPE_STRING:
            return pesec_value_as_boolean(pesec_string_value_greater(left.data.string, right));
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::greater: unsupported value type");
    }

    pesec_panic("value::greater: fuck");
}

pesec_value_t pesec_value_greater_equal(const pesec_value_t left, const pesec_value_t right)
{
    switch (left.type)
    {
        case PESEC_VALUE_TYPE_INTEGER:
            return pesec_value_as_boolean(pesec_integer_value_greater_equal(left.data.integer, right));
        case PESEC_VALUE_TYPE_FLOATING:
            return pesec_value_as_boolean(pesec_floating_value_greater_equal(left.data.floating, right));
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_boolean(pesec_vector_value_greater_equal(left.data.vector, right));
        case PESEC_VALUE_TYPE_STRING:
            return pesec_value_as_boolean(pesec_string_value_greater_equal(left.data.string, right));
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
        case PESEC_VALUE_TYPE_BOOLEAN:
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::greater_equal: unsupported value type");
    }

    pesec_panic("value::greater_equal: fuck");
}
