#include "include/runtime/value.h"

#include "include/common/panic.h"
#include "include/runtime/vector_value.h"

void pesec_value_print(FILE* stream, const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_NONE:
            fprintf(stream, "<fucking-nothing>");
            break;
        case PESEC_VALUE_TYPE_I64:
            fprintf(stream, "%" PRIi64, value.data.i64);
            break;
        case PESEC_VALUE_TYPE_F64:
            fprintf(stream, "%lf", value.data.f64);
            break;
        case PESEC_VALUE_TYPE_FUNCTION:
            fprintf(stream, "<function>");
            break;
        case PESEC_VALUE_TYPE_OBJECT:
            fprintf(stream, "<object>");
            break;
        case PESEC_VALUE_TYPE_VECTOR: {
            fprintf(stream, "{");
            for (size_t i = 0; i < value.data.vector->size; i++)
            {
                if (i > 0) fprintf(stream, ", ");
                pesec_value_print(stream, value.data.vector->items[i]);
            }
            fprintf(stream, "}");
            break;
        }
    }
}

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
    };
}

pesec_value_t pesec_value_as_i64(const int64_t data)
{
    return pesec_value_as_i64_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
}

pesec_value_t pesec_value_as_f64(const double data)
{
    return pesec_value_as_f64_with(data, PESEC_VALUE_CONTROL_FLOW_NONE);
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

pesec_value_t pesec_value_as_none()
{
    return pesec_value_as_none_with(PESEC_VALUE_CONTROL_FLOW_NONE);
}


pesec_value_t pesec_value_as_i64_with(const int64_t data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_I64,
        (pesec_value_data_t){
            .i64 = data
        },
        control_flow
    );
}

pesec_value_t pesec_value_as_f64_with(const double data, const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_F64,
        (pesec_value_data_t){
            .f64 = data
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

pesec_value_t pesec_value_as_none_with(const pesec_value_control_flow_t control_flow)
{
    return pesec_value_new(
        PESEC_VALUE_TYPE_NONE,
        (pesec_value_data_t){0},
        control_flow
    );
}


bool pesec_value_is_truthy(const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_NONE: return false;
        case PESEC_VALUE_TYPE_I64: return value.data.i64 != 0;
        case PESEC_VALUE_TYPE_F64: return value.data.f64 != 0;
        case PESEC_VALUE_TYPE_VECTOR: return value.data.vector->size != 0;
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT: return true;
    }

    pesec_panic("value::is_truthy: unknown value type (%d)", value.type);
}

pesec_value_t pesec_value_negative(const pesec_value_t value)
{
    switch (value.type)
    {
        case PESEC_VALUE_TYPE_NONE:
            pesec_panic("value::negative: none value can't be negatived");
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_as_i64(-value.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(-value.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
            pesec_panic("value::negative: function value can't be negatived");
        case PESEC_VALUE_TYPE_OBJECT:
            pesec_panic("value::negative: object value can't be negatived");
        case PESEC_VALUE_TYPE_VECTOR:
            pesec_panic("value::negative: vector value can't be negatived");
    }
    pesec_panic("value::negative: fuck");
}

pesec_value_t pesec_value_add(const pesec_value_t left, const pesec_value_t right)
{
    if (left.type != right.type)
        pesec_panic("value::add: both values should be same type");

    switch (left.type)
    {
        case PESEC_VALUE_TYPE_I64:
            return pesec_value_as_i64(left.data.i64 + right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 + right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 - right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 - right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 * right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 * right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 / right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 / right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 == right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 == right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 < right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 < right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 <= right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 <= right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 > right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 > right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
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
            return pesec_value_as_i64(left.data.i64 >= right.data.i64);
        case PESEC_VALUE_TYPE_F64:
            return pesec_value_as_f64(left.data.f64 >= right.data.f64);
        case PESEC_VALUE_TYPE_FUNCTION:
        case PESEC_VALUE_TYPE_OBJECT:
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_value_as_i64(0);
        case PESEC_VALUE_TYPE_NONE:
            return pesec_value_as_none();
    }
    pesec_panic("value::greater_equal: fuck");
}
