#include "include/array_value.h"

#include <stdlib.h>

#include "include/ast/ast_node.h"


array_value_t* array_value_new(value_t* values, const ull_t size)
{
    const auto array_value = (array_value_t*)malloc(sizeof(array_value_t));

    array_value->capacity = 256;
    array_value->size = size;
    array_value->values = values;

    return array_value;
}

value_t array_value_get(const array_value_t* array_value, const ull_t index)
{
    return array_value->values[index];
}

void array_value_set(const array_value_t* array_value, const ull_t index, const value_t value)
{
    array_value->values[index] = value;
}

array_value_t* array_value_concat(const array_value_t* left, const array_value_t* right)
{
    const ull_t size = left->size + right->size;
    const auto values = (value_t*)calloc(size, sizeof(value_t));

    for (int i = 0; i < left->size; i++) values[i] = left->values[i];
    for (int i = 0; i < right->size; i++) values[left->size + i] = right->values[i];

    return array_value_new(values, size);
}

void array_value_free(array_value_t* array_value)
{
    free(array_value->values);
    free(array_value);
}