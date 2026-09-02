#include "include/array_value.h"
#include "include/function_value.h"

#include <stdlib.h>
#include <string.h>

#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

static ull_t array_value_get_index(const array_value_t* array_value, long long index)
{
    if (index < 0)
        return array_value->size + index;
    return index;
}

array_value_t* array_value_new(value_t* values, const ull_t size)
{
    const auto array_value = (array_value_t*)malloc(sizeof(array_value_t));

    array_value->capacity = size;
    array_value->size = size;
    array_value->values = values;

    return array_value;
}

array_value_t* array_value_copy(const array_value_t* source)
{
    const auto values = (value_t*)malloc(sizeof(value_t) * source->size);
    memcpy(values, source->values, sizeof(value_t) * source->size);
    return array_value_new(values, source->size);
}

void array_value_free(array_value_t* array_value)
{
    free(array_value->values);
    free(array_value);
}

void array_value_set(const array_value_t* array_value, const long long index, const value_t value)
{
    array_value->values[array_value_get_index(array_value, index)] = value;
}

value_t array_value_get(const array_value_t* array_value, const long long index)
{
    return array_value->values[array_value_get_index(array_value, index)];
}

void array_value_push(array_value_t* array_value, const value_t value)
{
    if (array_value->size >= array_value->capacity)
    {
        array_value->capacity *= 2;
        const auto temp = (value_t*)realloc(array_value->values, array_value->capacity * sizeof(value_t));
        if (temp == nullptr) THROW("Failed to realloc memory to array_value");
        array_value->values = temp;
    }

    array_value->values[array_value->size] = value;
    ++array_value->size;
}

value_t array_value_pop(array_value_t* array_value)
{
    if (array_value->size == 0) THROW("Array value is empty");
    const value_t value = array_value->values[array_value->size - 1];
    --array_value->size;
    return value;
}

array_value_t* array_value_concat(const array_value_t* left, const array_value_t* right)
{
    const ull_t size = left->size + right->size;
    const auto values = (value_t*)calloc(size, sizeof(value_t));

    for (int i = 0; i < left->size; i++) values[i] = left->values[i];
    for (int i = 0; i < right->size; i++) values[left->size + i] = right->values[i];

    return array_value_new(values, size);
}

value_t array_value_resolve_field(const value_t array_value, const string_view_t name, context_t* context)
{
    if (string_view_equals_cstr(name, "size")) return ARRAY_METHOD_0(array_value_method_size);
    if (string_view_equals_cstr(name, "copy")) return ARRAY_METHOD_0(array_value_method_copy);
    if (string_view_equals_cstr(name, "push")) return ARRAY_METHOD(array_value_method_push, "value");
    if (string_view_equals_cstr(name, "pop")) return ARRAY_METHOD_0(array_value_method_pop);
    if (string_view_equals_cstr(name, "concat")) return ARRAY_METHOD(array_value_method_concat, "value");
    if (string_view_equals_cstr(name, "index_of")) return ARRAY_METHOD(array_value_method_index_of, "value");
    if (string_view_equals_cstr(name, "contains")) return ARRAY_METHOD(array_value_method_contains, "value");
    if (string_view_equals_cstr(name, "reverse")) return ARRAY_METHOD_0(array_value_method_reverse);
    if (string_view_equals_cstr(name, "clear")) return ARRAY_METHOD_0(array_value_method_clear);

    THROW("Variable '%s' does not have a field", name.data);
}

long long array_value_index_of(const array_value_t* array_value, const value_t value)
{
    for (ull_t i = 0; i < array_value->size; ++i)
    {
        if (value_operation_equals(array_value->values[i], value).data.as_bool)
            return (long long)i;
    }

    return -1;
}

bool array_value_contains(const array_value_t* array_value, const value_t value)
{
    return array_value_index_of(array_value, value) != -1;
}

void array_value_reverse(const array_value_t* array_value)
{
    if (array_value->size < 2)
        return;

    for (ull_t i = 0, j = array_value->size - 1; i < j; ++i, --j)
    {
        const value_t tmp = array_value->values[i];
        array_value->values[i] = array_value->values[j];
        array_value->values[j] = tmp;
    }
}

void array_value_clear(array_value_t* array_value)
{
    array_value->size = 0;
}

// ================================================================================

value_t array_value_method_size(const value_t array_value, context_t* context)
{
    return value_new_number(number_value_from_long_double(array_value.data.as_array->size));
}

value_t array_value_method_push(const value_t array_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    array_value_push(array_value.data.as_array, value->value);
    return value_new_number(number_value_zero());
}

value_t array_value_method_pop(const value_t array_value, context_t* context)
{
    array_value_pop(array_value.data.as_array);
    return value_new_number(number_value_zero());
}

value_t array_value_method_copy(const value_t array_value, context_t* context)
{
    return value_new_array(array_value_copy(array_value.data.as_array));
}

value_t array_value_method_concat(const value_t array_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    return value_new_array(array_value_concat(array_value.data.as_array, value->value.data.as_array));
}

value_t array_value_method_index_of(const value_t array_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    return value_new_number(number_value_from_long_double(array_value_index_of(array_value.data.as_array, value->value)));
}

value_t array_value_method_contains(const value_t array_value, context_t* context)
{
    const context_item_t* value = context_get(context, string_view_from("value"));
    return value_new_boolean(array_value_contains(array_value.data.as_array, value->value));
}

value_t array_value_method_reverse(const value_t array_value, context_t* context)
{
    array_value_reverse(array_value.data.as_array);
    return value_new_number(number_value_zero());
}

value_t array_value_method_clear(const value_t array_value, context_t* context)
{
    array_value_clear(array_value.data.as_array);
    return value_new_number(number_value_zero());
}