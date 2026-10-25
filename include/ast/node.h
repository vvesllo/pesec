#ifndef PESEC_NODE_H
#define PESEC_NODE_H

#include "literal_node.h"
#include "binary_node.h"
#include "unary_node.h"
#include "variable_node.h"
#include "call_node.h"
#include "assignment_node.h"
#include "sequence_node.h"
#include "definition_node.h"
#include "conditional_node.h"
#include "function_node.h"
#include "vector_node.h"
#include "object_node.h"
#include "index_access_node.h"
#include "field_access_node.h"
#include "range_node.h"
#include "for_node.h"
#include "while_node.h"
#include "control_flow_node.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;

typedef enum
{
    PESEC_NODE_TYPE_LITERAL,
    PESEC_NODE_TYPE_BINARY,
    PESEC_NODE_TYPE_UNARY,
    PESEC_NODE_TYPE_VARIABLE,
    PESEC_NODE_TYPE_CALL,
    PESEC_NODE_TYPE_ASSIGNMENT,
    PESEC_NODE_TYPE_SEQUENCE,
    PESEC_NODE_TYPE_DEFINITION,
    PESEC_NODE_TYPE_CONDITIONAL,
    PESEC_NODE_TYPE_FUNCTION,
    PESEC_NODE_TYPE_VECTOR,
    PESEC_NODE_TYPE_OBJECT,
    PESEC_NODE_TYPE_INDEX_ACCESS,
    PESEC_NODE_TYPE_FIELD_ACCESS,
    PESEC_NODE_TYPE_RANGE,
    PESEC_NODE_TYPE_FOR,
    PESEC_NODE_TYPE_WHILE,
    PESEC_NODE_TYPE_CONTROL_FLOW,
} pesec_node_type_t;

typedef union
{
    pesec_literal_node_t literal_node;
    pesec_binary_node_t binary_node;
    pesec_unary_node_t unary_node;
    pesec_variable_node_t variable_node;
    pesec_call_node_t call_node;
    pesec_assignment_node_t assignment_node;
    pesec_sequence_node_t sequence_node;
    pesec_definition_node_t definition_node;
    pesec_conditional_node_t conditional_node;
    pesec_function_node_t function_node;
    pesec_vector_node_t vector_node;
    pesec_object_node_t object_node;
    pesec_index_access_node_t index_access_node;
    pesec_field_access_node_t field_access_node;
    pesec_range_node_t range_node;
    pesec_for_node_t for_node;
    pesec_while_node_t while_node;
    pesec_control_flow_node_t control_flow_node;
} pesec_node_data_t;

typedef struct PESEC_NODE_STRUCT
{
    pesec_node_type_t type;
    pesec_node_data_t data;
} pesec_node_t;

pesec_node_t* pesec_node_new(pesec_node_type_t type, pesec_node_data_t data);

pesec_value_t pesec_node_evaluate(const pesec_node_t* node, pesec_environment_t* environment);

void pesec_node_destroy(pesec_node_t* node);

#endif // PESEC_NODE_H