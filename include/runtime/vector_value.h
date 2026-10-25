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

void pesec_vector_value_set(const pesec_vector_value_t* vector_value, const pesec_integer_value_t* index, pesec_value_t value);

pesec_value_t* pesec_vector_value_get(const pesec_vector_value_t* vector_value, const pesec_integer_value_t* index);

void pesec_vector_value_destroy(pesec_vector_value_t* vector_value);

void pesec_vector_value_print(FILE* stream, const pesec_vector_value_t* vector_value);


bool pesec_vector_value_is_truthy(const pesec_vector_value_t* vector_value);

pesec_value_t pesec_vector_value_negative(const pesec_vector_value_t* vector_value);

pesec_value_t pesec_vector_value_add(const pesec_vector_value_t* left, pesec_value_t right);

pesec_value_t pesec_vector_value_sub(const pesec_vector_value_t* left, pesec_value_t right);

pesec_value_t pesec_vector_value_mul(const pesec_vector_value_t* left, pesec_value_t right);

pesec_value_t pesec_vector_value_div(const pesec_vector_value_t* left, pesec_value_t right);

pesec_value_t pesec_vector_value_mod(const pesec_vector_value_t* left, pesec_value_t right);

bool pesec_vector_value_equal(const pesec_vector_value_t* left, pesec_value_t right);

bool pesec_vector_value_less(const pesec_vector_value_t* left, pesec_value_t right);

bool pesec_vector_value_less_equal(const pesec_vector_value_t* left, pesec_value_t right);

bool pesec_vector_value_greater(const pesec_vector_value_t* left, pesec_value_t right);

bool pesec_vector_value_greater_equal(const pesec_vector_value_t* left, pesec_value_t right);


pesec_value_t pesec_vector_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_VECTOR_VALUE_H
