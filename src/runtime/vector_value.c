#include "include/runtime/vector_value.h"

#include "include/ast/node.h"
#include "include/common/alloc.h"

pesec_vector_value_t pesec_vector_value_new()
{
    pesec_vector_value_t vector_value;

    vector_value.size = 0;
    vector_value.capacity = 64;
    vector_value.items = (pesec_value_t*)pesec_calloc(vector_value.capacity, sizeof(pesec_value_t));

    return vector_value;
}

void pesec_vector_value_push(pesec_vector_value_t* vector_value, pesec_value_t value)
{
    if (vector_value->size >= vector_value->capacity)
    {
        vector_value->capacity <<= 1;
        vector_value->items = (pesec_value_t*)pesec_realloc(vector_value->items, vector_value->capacity * sizeof(pesec_value_t));
    }

    vector_value->items[vector_value->size] = value;
    vector_value->size += 1;
}

void pesec_vector_value_destroy(const pesec_vector_value_t* vector_value)
{
    pesec_free(vector_value->items);
}