#ifndef PESEC_FUNCTION_CALL_NODE_H
#define PESEC_FUNCTION_CALL_NODE_H

#include "include/common/node_vector.h"
#include "include/common/string_view.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_string_view_t name;
    pesec_node_vector_t args;
} pesec_function_call_node_t;

pesec_node_t* pesec_function_call_node_new(pesec_string_view_t name, pesec_node_vector_t args);

pesec_value_t pesec_function_call_node_evaluate(const pesec_function_call_node_t* function_call_node, pesec_environment_t* environment);

void pesec_function_call_node_destroy(const pesec_function_call_node_t* function_call_node);

#endif // PESEC_FUNCTION_CALL_NODE_H