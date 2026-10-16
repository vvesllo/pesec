#ifndef PESEC_LITERAL_NODE_H
#define PESEC_LITERAL_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_value_t value;
} pesec_literal_node_t;

pesec_node_t* pesec_literal_node_new(pesec_value_t value);

pesec_value_t pesec_literal_node_evaluate(const pesec_literal_node_t* literal_node, pesec_environment_t* environment);

void pesec_literal_node_destroy(const pesec_literal_node_t* literal_node);

#endif // PESEC_LITERAL_NODE_H