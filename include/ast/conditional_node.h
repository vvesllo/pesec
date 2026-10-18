#ifndef PESEC_CONDITIONAL_NODE_H
#define PESEC_CONDITIONAL_NODE_H

#include "include/common/string_view.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* condition;
    pesec_node_t* then_body;
    pesec_node_t* else_body;
} pesec_conditional_node_t;

pesec_node_t* pesec_conditional_node_new(
    pesec_node_t* condition,
    pesec_node_t* then_body,
    pesec_node_t* else_body
);

pesec_value_t pesec_conditional_node_evaluate(
    const pesec_conditional_node_t* conditional_node,
    pesec_environment_t* environment
);

void pesec_conditional_node_destroy(const pesec_conditional_node_t* conditional_node);

#endif // PESEC_CONDITIONAL_NODE_H
