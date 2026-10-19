#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <inttypes.h>
#include <stdio.h>

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;

typedef struct PESEC_FUNCTION_VALUE_STRUCT pesec_function_value_t;
typedef struct PESEC_VECTOR_VALUE_STRUCT pesec_vector_value_t;
typedef struct PESEC_OBJECT_VALUE_STRUCT pesec_object_value_t;


typedef enum
{
    PESEC_VALUE_TYPE_NONE,
    PESEC_VALUE_TYPE_I64,
    PESEC_VALUE_TYPE_F64,
    PESEC_VALUE_TYPE_FUNCTION,
    PESEC_VALUE_TYPE_VECTOR,
    PESEC_VALUE_TYPE_OBJECT,
} pesec_value_type_t;

typedef enum
{
    PESEC_VALUE_CONTROL_FLOW_NONE,
    PESEC_VALUE_CONTROL_FLOW_CONTINUE,
    PESEC_VALUE_CONTROL_FLOW_BREAK,
    PESEC_VALUE_CONTROL_FLOW_RETURN,
} pesec_value_control_flow_t;

typedef union
{
    int64_t i64;
    double f64;
    pesec_function_value_t* function;
    pesec_vector_value_t* vector;
    pesec_object_value_t* object;
} pesec_value_data_t;

typedef struct PESEC_VALUE_STRUCT
{
    pesec_value_type_t type;
    pesec_value_data_t data;
    pesec_value_control_flow_t control_flow;
} pesec_value_t;

void pesec_value_print(FILE* stream, pesec_value_t value);

pesec_value_t pesec_value_i64(int64_t data);

pesec_value_t pesec_value_f64(double data);

pesec_value_t pesec_value_function(pesec_function_value_t* data);

pesec_value_t pesec_value_vector(pesec_vector_value_t* data);

pesec_value_t pesec_value_object(pesec_object_value_t* data);

pesec_value_t pesec_value_none();

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
