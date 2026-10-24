#ifndef PESEC_FUNCTION_VALUE_H
#define PESEC_FUNCTION_VALUE_H

#include "include/common/parameter_vector.h"
#include "value.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;
typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef pesec_value_t (*pesec_function_value_native_t)(pesec_environment_t*);
typedef pesec_node_t* pesec_function_value_node_t;

typedef struct PESEC_METHOD_STRUCT
{
    pesec_value_t self;
    pesec_value_t (*method)(pesec_value_t*, pesec_environment_t*);
} pesec_function_value_method_t;

typedef enum
{
    PESEC_FUNCTION_VALUE_TYPE_NATIVE,
    PESEC_FUNCTION_VALUE_TYPE_NODE,
    PESEC_FUNCTION_VALUE_TYPE_METHOD,
} pesec_function_value_type_t;

typedef union
{
    pesec_function_value_native_t native;
    pesec_function_value_node_t node;
    pesec_function_value_method_t method;
} pesec_function_value_data_t;

typedef struct PESEC_FUNCTION_VALUE_STRUCT
{
    pesec_function_value_type_t type;
    pesec_function_value_data_t data;
    pesec_parameter_vector_t parameter_vector;
    pesec_environment_t* parent_environment;
} pesec_function_value_t;

pesec_function_value_t* pesec_function_value_new(
    pesec_function_value_type_t type,
    pesec_function_value_data_t data,
    pesec_parameter_vector_t parameter_vector,
    pesec_environment_t* parent_environment
);

pesec_value_t pesec_function_value_call(pesec_function_value_t* function_value, pesec_environment_t* environment);

void pesec_function_value_destroy(pesec_function_value_t* function_value);

void pesec_function_value_print(FILE* stream, const pesec_function_value_t* function_value);

#endif // PESEC_FUNCTION_VALUE_H
