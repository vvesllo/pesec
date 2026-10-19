#ifndef PESEC_UNARY_NODE_H
#define PESEC_UNARY_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef enum
{
    PESEC_UNARY_NODE_OPERATION_NEGATIVE,
} pesec_unary_node_operation_t;

typedef struct
{
    pesec_unary_node_operation_t operation;
    pesec_node_t* operand;
} pesec_unary_node_t;

pesec_node_t* pesec_unary_node_new(pesec_unary_node_operation_t operation, pesec_node_t* operand);

pesec_value_t pesec_unary_node_evaluate(const pesec_unary_node_t* unary_node, pesec_environment_t* environment);

void pesec_unary_node_destroy(const pesec_unary_node_t* unary_node);

#endif // PESEC_UNARY_NODE_H
