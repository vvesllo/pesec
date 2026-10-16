#ifndef PESEC_BINARY_NODE_H
#define PESEC_BINARY_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef enum
{
    PESEC_BINARY_NODE_OPERATION_ADD,
    PESEC_BINARY_NODE_OPERATION_SUB,
    PESEC_BINARY_NODE_OPERATION_MUL,
    PESEC_BINARY_NODE_OPERATION_DIV,
} pesec_binary_node_operation_t;

typedef struct
{
    pesec_binary_node_operation_t operation;
    pesec_node_t* left;
    pesec_node_t* right;
} pesec_binary_node_t;

pesec_node_t* pesec_binary_node_new(pesec_binary_node_operation_t operation, pesec_node_t* left, pesec_node_t* right);

pesec_value_t pesec_binary_node_evaluate(const pesec_binary_node_t* binary_node, pesec_environment_t* environment);

void pesec_binary_node_destroy(const pesec_binary_node_t* binary_node);

#endif // PESEC_BINARY_NODE_H
