#include "include/ast/node.h"

pesec_node_t* pesec_definition_node_new(
    pesec_string_view_t name,
    pesec_node_t* value,
    bool immutable
)
{
    const auto definition_node = (pesec_definition_node_t)
    {
        .name = name,
        .value = value,
        .immutable = immutable,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_DEFINITION,
        (pesec_node_data_t){
            .definition_node = definition_node
        }
    );
}

pesec_value_t pesec_definition_node_evaluate(
    const pesec_definition_node_t* definition_node,
    pesec_environment_t* environment
)
{
    const pesec_value_t value = pesec_node_evaluate(definition_node->value, environment);
    pesec_environment_push_variable(
        environment,
        definition_node->name,
        value,
        definition_node->immutable
    );
    return value;
}

void pesec_definition_node_destroy(const pesec_definition_node_t* definition_node)
{
    // nothing to do
}
