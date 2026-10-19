#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <inttypes.h>
#include <stdio.h>

#include "function_value.h"
#include "vector_value.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;

typedef enum
{
    PESEC_VALUE_TYPE_I64,
    PESEC_VALUE_TYPE_FUNCTION,
    PESEC_VALUE_TYPE_VECTOR,
} pesec_value_type_t;

typedef union
{
    int64_t integer;
    pesec_function_value_t function;
    pesec_vector_value_t vector;
} pesec_value_data_t;

typedef struct PESEC_VALUE_STRUCT
{
    pesec_value_type_t type;
    pesec_value_data_t data;
} pesec_value_t;

void pesec_value_print(FILE* stream, pesec_value_t value);

pesec_value_t pesec_value_i64(int64_t data);

pesec_value_t pesec_value_function(pesec_function_value_t data);

pesec_value_t pesec_value_vector(pesec_vector_value_t data);

bool pesec_value_is_truthy(pesec_value_t value);

pesec_value_t pesec_value_negative(pesec_value_t value);

pesec_value_t pesec_value_add(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_sub(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_mul(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_div(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_equal(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_less(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_less_equal(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_greater(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_greater_equal(pesec_value_t left, pesec_value_t right);

#endif // PESEC_VALUE_H
