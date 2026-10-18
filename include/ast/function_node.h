#ifndef PESEC_FUNCTION_NODE_H
#define PESEC_FUNCTION_NODE_H

#include "include/common/parameter_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* body;
    pesec_parameter_vector_t parameter_vector;
} pesec_function_node_t;

pesec_node_t* pesec_function_node_new(pesec_node_t* body, pesec_parameter_vector_t parameter_vector);

pesec_value_t pesec_function_node_evaluate(const pesec_function_node_t* function_node, pesec_environment_t* environment);

void pesec_function_node_destroy(const pesec_function_node_t* function_node);

#endif // PESEC_FUNCTION_NODE_H