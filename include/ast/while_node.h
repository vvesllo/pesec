#ifndef PESEC_WHILE_NODE_H
#define PESEC_WHILE_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* condition;
    pesec_node_t* body;
} pesec_while_node_t;

pesec_node_t* pesec_while_node_new(pesec_node_t* condition, pesec_node_t* body);

pesec_value_t pesec_while_node_evaluate(const pesec_while_node_t* while_node, pesec_environment_t* environment);

void pesec_while_node_destroy(const pesec_while_node_t* while_node);

#endif // PESEC_WHILE_NODE_H
