#ifndef PESEC_FUNCTION_VALUE_H
#define PESEC_FUNCTION_VALUE_H

#include "include/common/parameter_vector.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;
typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef pesec_value_t (*pesec_function_value_native_t)(pesec_environment_t*);
typedef pesec_node_t* pesec_function_value_node_t;


typedef enum
{
    PESEC_FUNCTION_VALUE_TYPE_NATIVE,
    PESEC_FUNCTION_VALUE_TYPE_NODE,
} pesec_function_value_type_t;

typedef union
{
    pesec_function_value_native_t native;
    pesec_function_value_node_t node;
} pesec_function_value_data_t;

typedef struct
{
    pesec_function_value_type_t type;
    pesec_function_value_data_t data;
    pesec_parameter_vector_t parameter_vector;
    pesec_environment_t* parent_environment;
} pesec_function_value_t;

pesec_value_t pesec_function_value_call(const pesec_function_value_t* function_value, pesec_environment_t* environment);

#endif // PESEC_FUNCTION_VALUE_H
