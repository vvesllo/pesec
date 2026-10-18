#include "include/common/parameter_vector.h"

#include "include/common/alloc.h"

pesec_parameter_vector_t pesec_parameter_vector_new()
{
    pesec_parameter_vector_t parameter_vector;

    parameter_vector.size = 0;
    parameter_vector.capacity = 64;
    parameter_vector.items = (pesec_parameter_t*)pesec_calloc(parameter_vector.capacity, sizeof(pesec_parameter_t));

    return parameter_vector;
}

pesec_parameter_vector_t pesec_parameter_vector_from(const pesec_parameter_t* items, const size_t size)
{
    pesec_parameter_vector_t parameter_vector = pesec_parameter_vector_new();

    for (size_t i = 0; i < size; i++)
        pesec_parameter_vector_push(&parameter_vector, items[i]);

    return parameter_vector;
}

void pesec_parameter_vector_push(pesec_parameter_vector_t* parameter_vector, const pesec_parameter_t parameter)
{
    if (parameter_vector->size >= parameter_vector->capacity)
    {
        parameter_vector->capacity <<= 1;
        parameter_vector->items = (pesec_parameter_t*)pesec_realloc(parameter_vector->items,
                                                                    parameter_vector->capacity * sizeof(
                                                                        pesec_parameter_t));
    }

    parameter_vector->items[parameter_vector->size] = parameter;
    parameter_vector->size += 1;
}

void pesec_parameter_vector_destroy(const pesec_parameter_vector_t* parameter_vector)
{
    pesec_free(parameter_vector->items);
}
