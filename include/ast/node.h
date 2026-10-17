#ifndef PESEC_NODE_H
#define PESEC_NODE_H

#include "include/runtime/value.h"
#include "include/runtime/environment.h"
#include "literal_node.h"
#include "binary_node.h"
#include "variable_node.h"
#include "function_call_node.h"

typedef enum
{
    PESEC_NODE_TYPE_LITERAL,
    PESEC_NODE_TYPE_BINARY,
    PESEC_NODE_TYPE_VARIABLE,
    PESEC_NODE_TYPE_FUNCTION_CALL,
} pesec_node_type_t;

typedef union
{
    pesec_literal_node_t literal_node;
    pesec_binary_node_t binary_node;
    pesec_variable_node_t variable_node;
    pesec_function_call_node_t function_call_node;
} pesec_node_data_t;

typedef struct PESEC_NODE_STRUCT
{
    pesec_node_type_t type;
    pesec_node_data_t data;
} pesec_node_t;

pesec_node_t* pesec_node_new(pesec_node_type_t type, pesec_node_data_t data);

pesec_value_t pesec_node_evaluate(const pesec_node_t* node, pesec_environment_t* environment);

void pesec_node_destroy(const pesec_node_t* node);

#endif // PESEC_NODE_H