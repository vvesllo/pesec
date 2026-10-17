#ifndef PESEC_PARAMETER_VECTOR_H
#define PESEC_PARAMETER_VECTOR_H

#include "string_view.h"

typedef struct
{
    pesec_string_view_t name;
} pesec_parameter_t;

typedef struct
{
    pesec_parameter_t* items;
    size_t capacity;
    size_t size;
} pesec_parameter_vector_t;

pesec_parameter_vector_t pesec_parameter_vector_new();

void pesec_parameter_vector_push(pesec_parameter_vector_t* parameter_vector, pesec_parameter_t parameter);

void pesec_parameter_vector_destroy(const pesec_parameter_vector_t* parameter_vector);

#endif // PESEC_PARAMETER_VECTOR_H
