#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <inttypes.h>
#include <stddef.h>

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;

typedef enum
{
    PESEC_VALUE_TYPE_I64,
    PESEC_VALUE_TYPE_FUNCTION,
} pesec_value_type_t;

// argc, argv, env
typedef pesec_value_t (*pesec_function_value_t)(size_t, pesec_value_t*, pesec_environment_t*);

typedef union
{
    int64_t integer;
    pesec_function_value_t function;
} pesec_value_data_t;

typedef struct PESEC_VALUE_STRUCT
{
    pesec_value_type_t type;
    pesec_value_data_t data;
} pesec_value_t;

pesec_value_t pesec_value_i64(int64_t data);

pesec_value_t pesec_value_function(pesec_function_value_t data);

pesec_value_t pesec_value_add(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_sub(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_mul(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_div(pesec_value_t left, pesec_value_t right);

#endif // PESEC_VALUE_H
