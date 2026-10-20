#include "include/ast/node.h"
#include "include/common/alloc.h"

pesec_node_t* pesec_node_new(const pesec_node_type_t type, const pesec_node_data_t data)
{
    const auto node = (pesec_node_t*)pesec_malloc(sizeof(pesec_node_t));
    node->type = type;
    node->data = data;
    return node;
}

pesec_value_t pesec_node_evaluate(const pesec_node_t* node, pesec_environment_t* environment)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: return pesec_literal_node_evaluate(&node->data.literal_node, environment);
        case PESEC_NODE_TYPE_BINARY: return pesec_binary_node_evaluate(&node->data.binary_node, environment);
        case PESEC_NODE_TYPE_UNARY: return pesec_unary_node_evaluate(&node->data.unary_node, environment);
        case PESEC_NODE_TYPE_VARIABLE: return pesec_variable_node_evaluate(&node->data.variable_node, environment);
        case PESEC_NODE_TYPE_CALL: return pesec_call_node_evaluate(&node->data.call_node, environment);
        case PESEC_NODE_TYPE_ASSIGNMENT: return pesec_assignment_node_evaluate(&node->data.assignment_node, environment);
        case PESEC_NODE_TYPE_SEQUENCE: return pesec_sequence_node_evaluate(&node->data.sequence_node, environment);
        case PESEC_NODE_TYPE_DEFINITION: return pesec_definition_node_evaluate(&node->data.definition_node, environment);
        case PESEC_NODE_TYPE_CONDITIONAL: return pesec_conditional_node_evaluate(&node->data.conditional_node, environment);
        case PESEC_NODE_TYPE_FUNCTION: return pesec_function_node_evaluate(&node->data.function_node, environment);
        case PESEC_NODE_TYPE_VECTOR: return pesec_vector_node_evaluate(&node->data.vector_node, environment);
        case PESEC_NODE_TYPE_OBJECT: return pesec_object_node_evaluate(&node->data.object_node, environment);
        case PESEC_NODE_TYPE_INDEX_ACCESS: return pesec_index_access_node_evaluate(&node->data.index_access_node, environment);
        case PESEC_NODE_TYPE_FIELD_ACCESS: return pesec_field_access_node_evaluate(&node->data.field_access_node, environment);
        case PESEC_NODE_TYPE_RANGE: return pesec_range_node_evaluate(&node->data.range_node, environment);
        case PESEC_NODE_TYPE_FOR: return pesec_for_node_evaluate(&node->data.for_node, environment);
        case PESEC_NODE_TYPE_CONTROL_FLOW: return pesec_control_flow_node_evaluate(&node->data.control_flow_node, environment);
    }

    return pesec_value_as_i64(0);
}

void pesec_node_destroy(const pesec_node_t* node)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: pesec_literal_node_destroy(&node->data.literal_node); break;
        case PESEC_NODE_TYPE_BINARY: pesec_binary_node_destroy(&node->data.binary_node); break;
        case PESEC_NODE_TYPE_UNARY: pesec_unary_node_destroy(&node->data.unary_node); break;
        case PESEC_NODE_TYPE_VARIABLE: pesec_variable_node_destroy(&node->data.variable_node); break;
        case PESEC_NODE_TYPE_CALL: pesec_call_node_destroy(&node->data.call_node); break;
        case PESEC_NODE_TYPE_ASSIGNMENT: pesec_assignment_node_destroy(&node->data.assignment_node); break;
        case PESEC_NODE_TYPE_SEQUENCE: pesec_sequence_node_destroy(&node->data.sequence_node); break;
        case PESEC_NODE_TYPE_DEFINITION: pesec_definition_node_destroy(&node->data.definition_node); break;
        case PESEC_NODE_TYPE_CONDITIONAL: pesec_conditional_node_destroy(&node->data.conditional_node); break;
        case PESEC_NODE_TYPE_FUNCTION: pesec_function_node_destroy(&node->data.function_node); break;
        case PESEC_NODE_TYPE_VECTOR: pesec_vector_node_destroy(&node->data.vector_node); break;
        case PESEC_NODE_TYPE_OBJECT: pesec_object_node_destroy(&node->data.object_node); break;
        case PESEC_NODE_TYPE_INDEX_ACCESS: pesec_index_access_node_destroy(&node->data.index_access_node); break;
        case PESEC_NODE_TYPE_FIELD_ACCESS: pesec_field_access_node_destroy(&node->data.field_access_node); break;
        case PESEC_NODE_TYPE_RANGE: pesec_range_node_destroy(&node->data.range_node); break;
        case PESEC_NODE_TYPE_FOR: pesec_for_node_destroy(&node->data.for_node); break;
        case PESEC_NODE_TYPE_CONTROL_FLOW: pesec_control_flow_node_destroy(&node->data.control_flow_node); break;
    }
}
