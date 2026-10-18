#ifndef PESEC_CALL_NODE_H
#define PESEC_CALL_NODE_H

#include "include/common/node_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* callee;
    pesec_node_vector_t args;
} pesec_call_node_t;

pesec_node_t* pesec_call_node_new(pesec_node_t* callee, pesec_node_vector_t args);

pesec_value_t pesec_call_node_evaluate(const pesec_call_node_t* call_node, pesec_environment_t* environment);

void pesec_call_node_destroy(const pesec_call_node_t* call_node);

#endif // PESEC_CALL_NODE_H
