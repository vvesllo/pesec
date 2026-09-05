#include "include/vector_value.h"
#include "include/function_value.h"

#include <stdlib.h>
#include <string.h>

#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

static ull_t vector_value_get_index(const vector_value_t* vector_value, long long index)
{
    if (index < 0)
        return vector_value->size + index;
    return index;
}

vector_value_t* vector_value_new(value_t* values, const ull_t size)
{
    const auto vector_value = (vector_value_t*)malloc(sizeof(vector_value_t));

    vector_value->capacity = size;
    vector_value->size = size;
    vector_value->values = values;

    return vector_value;
}

vector_value_t* vector_value_new_size(const ull_t size)
{
    const auto vector_value = (vector_value_t*)malloc(sizeof(vector_value_t));

    vector_value->capacity = size + 1;
    vector_value->size = size;
    vector_value->values = (value_t*)calloc(vector_value->capacity, sizeof(value_t));

    return vector_value;
}

vector_value_t* vector_value_copy(const vector_value_t* source)
{
    const auto values = (value_t*)malloc(sizeof(value_t) * source->size);
    memcpy(values, source->values, sizeof(value_t) * source->size);
    return vector_value_new(values, source->size);
}

void vector_value_free(vector_value_t* vector_value)
{
    free(vector_value->values);
    free(vector_value);
}

void vector_value_set(const vector_value_t* vector_value, const long long index, const value_t value)
{
    vector_value->values[vector_value_get_index(vector_value, index)] = value;
}

value_t vector_value_get(const vector_value_t* vector_value, const long long index)
{
    return vector_value->values[vector_value_get_index(vector_value, index)];
}

void vector_value_push(vector_value_t* vector_value, const value_t value)
{
    if (vector_value->size >= vector_value->capacity)
    {
        vector_value->capacity *= 2;
        const auto temp = (value_t*)realloc(vector_value->values, vector_value->capacity * sizeof(value_t));
        if (temp == nullptr) THROW("Failed to realloc memory to vector_value");
        vector_value->values = temp;
    }

    vector_value->values[vector_value->size] = value;
    ++vector_value->size;
}

value_t vector_value_pop(vector_value_t* vector_value)
{
    if (vector_value->size == 0) THROW("Vector value is empty");
    const value_t value = vector_value->values[vector_value->size - 1];
    --vector_value->size;
    return value;
}

value_t vector_value_get_fields(const vector_value_t* vector_value)
{
    vector_value_t* fields_vector = vector_value_new_size(0);
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("size")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("push")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("pop")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("concat")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("index_of")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("contains")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("clear")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("copy")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("reverse")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("map")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("filter")));
    return value_new_vector(fields_vector);
}

value_t vector_value_resolve_field(const value_t vector_value, const string_view_t name, context_t* context)
{
    if (string_view_equals_cstr(name, "size")) return VECTOR_METHOD_0(vector_value_method_size);
    if (string_view_equals_cstr(name, "push")) return VECTOR_METHOD(vector_value_method_push, "value");
    if (string_view_equals_cstr(name, "pop")) return VECTOR_METHOD_0(vector_value_method_pop);
    if (string_view_equals_cstr(name, "concat")) return VECTOR_METHOD(vector_value_method_concat, "value");
    if (string_view_equals_cstr(name, "index_of")) return VECTOR_METHOD(vector_value_method_index_of, "value");
    if (string_view_equals_cstr(name, "contains")) return VECTOR_METHOD(vector_value_method_contains, "value");
    if (string_view_equals_cstr(name, "clear")) return VECTOR_METHOD_0(vector_value_method_clear);
    if (string_view_equals_cstr(name, "copy")) return VECTOR_METHOD_0(vector_value_method_copy);
    if (string_view_equals_cstr(name, "reverse")) return VECTOR_METHOD_0(vector_value_method_reverse);
    if (string_view_equals_cstr(name, "map")) return VECTOR_METHOD(vector_value_method_map, "function");
    if (string_view_equals_cstr(name, "filter")) return VECTOR_METHOD(vector_value_method_filter, "function");

    THROW("Vector '%s' does not have a field", name.data);
}

long long vector_value_index_of(const vector_value_t* vector_value, const value_t value)
{
    for (ull_t i = 0; i < vector_value->size; ++i)
    {
        if (value_operation_equals(vector_value->values[i], value).data.as_bool)
            return (long long)i;
    }

    return -1;
}

bool vector_value_contains(const vector_value_t* vector_value, const value_t value)
{
    return vector_value_index_of(vector_value, value) != -1;
}

void vector_value_clear(vector_value_t* vector_value)
{
    vector_value->size = 0;
}

vector_value_t* vector_value_concat(const vector_value_t* left, const vector_value_t* right)
{
    const ull_t size = left->size + right->size;
    const auto values = (value_t*)calloc(size, sizeof(value_t));

    for (int i = 0; i < left->size; i++) values[i] = left->values[i];
    for (int i = 0; i < right->size; i++) values[left->size + i] = right->values[i];

    return vector_value_new(values, size);
}

vector_value_t* vector_value_reverse(const vector_value_t* source)
{
    vector_value_t* vector_value = vector_value_copy(source);
    if (vector_value->size < 2) return vector_value;

    for (ull_t i = 0, j = vector_value->size - 1; i < j; ++i, --j)
    {
        const value_t tmp = vector_value->values[i];
        vector_value->values[i] = vector_value->values[j];
        vector_value->values[j] = tmp;
    }

    return vector_value;
}

// ================================================================================

value_t vector_value_method_size(const value_t vector_value, context_t* context)
{
    return value_new_number(number_value_from_long_double(vector_value.data.as_vector->size));
}

value_t vector_value_method_push(const value_t vector_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    vector_value_push(vector_value.data.as_vector, value->value);
    return value_new_number(number_value_zero());
}

value_t vector_value_method_pop(const value_t vector_value, context_t* context)
{
    vector_value_pop(vector_value.data.as_vector);
    return value_new_number(number_value_zero());
}

value_t vector_value_method_copy(const value_t vector_value, context_t* context)
{
    return value_new_vector(vector_value_copy(vector_value.data.as_vector));
}

value_t vector_value_method_index_of(const value_t vector_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    return value_new_number(number_value_from_long_double(vector_value_index_of(vector_value.data.as_vector, value->value)));
}

value_t vector_value_method_contains(const value_t vector_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    return value_new_boolean(vector_value_contains(vector_value.data.as_vector, value->value));
}

value_t vector_value_method_clear(const value_t vector_value, context_t* context)
{
    vector_value_clear(vector_value.data.as_vector);
    return value_new_number(number_value_zero());
}

value_t vector_value_method_concat(const value_t vector_value, context_t* context)
{
    const value_t value = context_get(context, string_view_from("value"))->value;
    return value_new_vector(vector_value_concat(vector_value.data.as_vector, value.data.as_vector));
}

value_t vector_value_method_reverse(const value_t vector_value, context_t* context)
{
    return value_new_vector(vector_value_reverse(vector_value.data.as_vector));
}

value_t vector_value_method_map(const value_t vector_value, context_t* context)
{
    const value_t function = context_get(context, string_view_from("function"))->value;

    if (function.type != VALUE_TYPE_FUNCTION) THROW("map argument must be a function");

    const vector_value_t* source = vector_value.data.as_vector;

    vector_value_t* vector_value_mapped = vector_value_new(nullptr, 0);
    vector_value_mapped->capacity = source->size > 0 ? source->size : 1;
    vector_value_mapped->values = (value_t*)malloc(sizeof(value_t) * vector_value_mapped->capacity);
    vector_value_mapped->size = 0;

    string_view_t param_name = string_view_from("it");
    if (function.data.as_function->parameter && function.data.as_function->parameter->count > 0)
        param_name = function.data.as_function->parameter->parameters->value;

    context_t* local_context = context_new(function.data.as_function->parent_context);
    context_push(local_context, param_name, value_new_null(), false);

    for (long long i = 0; i < source->size; ++i)
    {
        const value_t current_element = vector_value_get(source, i);
        context_set(local_context, param_name, current_element);

        value_t result = function_value_call(function.data.as_function, local_context);
        result.control_flow = CONTROL_FLOW_NONE;

        vector_value_push(vector_value_mapped, result);
    }

    context_free(local_context);

    return value_new_vector(vector_value_mapped);
}

value_t vector_value_method_filter(const value_t vector_value, context_t* context)
{

    const value_t function = context_get(context, string_view_from("function"))->value;

    if (function.type != VALUE_TYPE_FUNCTION) THROW("map argument must be a function");

    const vector_value_t* source = vector_value.data.as_vector;

    vector_value_t* vector_value_filtered = vector_value_new(nullptr, 0);
    vector_value_filtered->capacity = source->size > 0 ? source->size : 1;
    vector_value_filtered->values = (value_t*)malloc(sizeof(value_t) * vector_value_filtered->capacity);
    vector_value_filtered->size = 0;

    string_view_t param_name = string_view_from("it");
    if (function.data.as_function->parameter && function.data.as_function->parameter->count > 0)
        param_name = function.data.as_function->parameter->parameters->value;

    context_t* local_context = context_new(function.data.as_function->parent_context);
    context_push(local_context, param_name, value_new_null(), false);

    for (long long i = 0; i < source->size; ++i)
    {
        const value_t current_element = vector_value_get(source, i);
        context_set(local_context, param_name, current_element);

        const value_t result = function_value_call(function.data.as_function, local_context);

        if (result.data.as_bool)
            vector_value_push(vector_value_filtered, current_element);
    }

    context_free(local_context);

    return value_new_vector(vector_value_filtered);
}