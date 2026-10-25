#include "include/runtime/vector_value.h"

#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/number_value.h"

static size_t pesec_vector_value_get_index(const pesec_vector_value_t* vector_value, const pesec_number_value_t* index)
{
    const double index_value = pesec_number_value_get(index);

    if (index_value < 0)
    {
        const size_t positive_index = (size_t)((double)vector_value->size + index_value);
        if (positive_index < 0)
            pesec_panic("index_access_node::evaluate: index out of range");
        return (size_t)positive_index;
    }

    if ((size_t)index_value >= vector_value->size)
        pesec_panic("index_access_node::evaluate: index out of range");

    return (size_t)index_value;
}

static int pesec_vector_value_compare(const pesec_vector_value_t* left, const pesec_vector_value_t* right)
{
    const size_t min_size = left->size < right->size ? left->size : right->size;

    for (size_t i = 0; i < min_size; i++)
    {
        if (pesec_value_less(left->items[i], right->items[i]).data.boolean)
            return -1;

        if (pesec_value_greater(left->items[i], right->items[i]).data.boolean)
            return 1;
    }

    if (left->size < right->size) return -1;
    if (left->size > right->size) return 1;
    return 0;
}

pesec_vector_value_t* pesec_vector_value_new()
{
    const auto vector_value = (pesec_vector_value_t*)pesec_malloc(sizeof(pesec_vector_value_t));

    vector_value->size = 0;
    vector_value->capacity = 64;
    vector_value->items = (pesec_value_t*)pesec_calloc(vector_value->capacity, sizeof(pesec_value_t));

    return vector_value;
}

void pesec_vector_value_push(pesec_vector_value_t* vector_value, const pesec_value_t value)
{
    if (vector_value->size >= vector_value->capacity)
    {
        vector_value->capacity <<= 1;
        vector_value->items = (pesec_value_t*)pesec_realloc(
            vector_value->items,
            vector_value->capacity * sizeof(pesec_value_t)
        );
    }

    vector_value->items[vector_value->size] = value;
    vector_value->size += 1;
}

void pesec_vector_value_set(
    const pesec_vector_value_t* vector_value,
    const pesec_number_value_t* index,
    const pesec_value_t value
)
{
    vector_value->items[pesec_vector_value_get_index(vector_value, index)] = value;
}

pesec_value_t* pesec_vector_value_get(const pesec_vector_value_t* vector_value, const pesec_number_value_t* index)
{
    return &vector_value->items[pesec_vector_value_get_index(vector_value, index)];
}

void pesec_vector_value_destroy(pesec_vector_value_t* vector_value)
{
    pesec_free(vector_value->items);
    pesec_free(vector_value);
}

void pesec_vector_value_print(FILE* stream, const pesec_vector_value_t* vector_value)
{
    fprintf(stream, "{");
    for (size_t i = 0; i < vector_value->size; i++)
    {
        if (i > 0) fprintf(stream, ", ");
        pesec_value_print(stream, vector_value->items[i]);
    }
    fprintf(stream, "}");
}

bool pesec_vector_value_is_truthy(const pesec_vector_value_t* vector_value)
{
    return vector_value->size > 0;
}

pesec_value_t pesec_vector_value_negative(const pesec_vector_value_t* vector_value)
{
    (void)vector_value;
    pesec_panic("vector_value::negative: unsupported");
}

pesec_value_t pesec_vector_value_add(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::add: right operand must be vector");

    const pesec_vector_value_t* right_vector = right.data.vector;
    pesec_vector_value_t* result = pesec_vector_value_new();

    for (size_t i = 0; i < left->size; i++)
        pesec_vector_value_push(result, left->items[i]);

    for (size_t i = 0; i < right_vector->size; i++)
        pesec_vector_value_push(result, right_vector->items[i]);

    return pesec_value_as_vector(result);
}

pesec_value_t pesec_vector_value_sub(const pesec_vector_value_t* left, const pesec_value_t right)
{
    pesec_panic("vector_value::sub: unsupported");
}

pesec_value_t pesec_vector_value_mul(const pesec_vector_value_t* left, const pesec_value_t right)
{
    pesec_panic("vector_value::mul: unsupported");
}

pesec_value_t pesec_vector_value_div(const pesec_vector_value_t* left, const pesec_value_t right)
{
    pesec_panic("vector_value::div: unsupported");
}

pesec_value_t pesec_vector_value_mod(const pesec_vector_value_t* left, const pesec_value_t right)
{
    pesec_panic("vector_value::mod: unsupported");
}

bool pesec_vector_value_equal(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::equal: right operand must be vector");

    const pesec_vector_value_t* right_vector = right.data.vector;

    if (left->size != right_vector->size)
        return false;

    for (size_t i = 0; i < left->size; i++)
    {
        if (!pesec_value_equal(left->items[i], right_vector->items[i]).data.boolean)
            return false;
    }

    return true;
}

bool pesec_vector_value_less(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::less: right operand must be vector");

    return pesec_vector_value_compare(left, right.data.vector) < 0;
}

bool pesec_vector_value_less_equal(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::less_equal: right operand must be vector");

    return pesec_vector_value_compare(left, right.data.vector) <= 0;
}

bool pesec_vector_value_greater(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::greater: right operand must be vector");

    return pesec_vector_value_compare(left, right.data.vector) > 0;
}

bool pesec_vector_value_greater_equal(const pesec_vector_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("vector_value::greater_equal: right operand must be vector");

    return pesec_vector_value_compare(left, right.data.vector) >= 0;
}

static pesec_value_t pesec_vector_value_method_size(pesec_value_t* self, pesec_environment_t* environment)
{
    pesec_value_t size = pesec_value_as_number(pesec_number_value_new((double)self->data.vector->size));
    size.control_flow = PESEC_VALUE_CONTROL_FLOW_RETURN;
    return size;
}

static pesec_value_t pesec_vector_value_method_push_back(pesec_value_t* self, pesec_environment_t* environment)
{
    const pesec_variable_t* variable = pesec_environment_get_variable(
        environment,
        pesec_string_view_from("value")
    );
    if (!variable)
        pesec_panic("vector::push_back: missing `value` argument");

    pesec_vector_value_push(self->data.vector, variable->value);
    return pesec_value_as_none();
}

pesec_value_t pesec_vector_value_method_handler(
    pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    if (pesec_string_view_equals_cstr(field, "size"))
        return pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_vector_value_method_size,
            },
            pesec_parameter_vector_new(),
            environment
        ));
    if (pesec_string_view_equals_cstr(field, "push_back"))
        return pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_vector_value_method_push_back,
            },
            pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            environment
        ));

    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}
