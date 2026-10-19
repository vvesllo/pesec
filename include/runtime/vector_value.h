#ifndef PESEC_VECTOR_VALUE_H
#define PESEC_VECTOR_VALUE_H
#include "function_value.h"

typedef struct PESEC_VECTOR_VALUE_STRUCT
{
    pesec_value_t* items;
    size_t capacity;
    size_t size;
} pesec_vector_value_t;

pesec_vector_value_t* pesec_vector_value_new();

void pesec_vector_value_push(pesec_vector_value_t* vector_value, pesec_value_t value);

void pesec_vector_value_destroy(const pesec_vector_value_t* vector_value);

pesec_value_t pesec_vector_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_VECTOR_VALUE_H
