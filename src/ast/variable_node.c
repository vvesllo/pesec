#include "include/ast/node.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_variable_node_new(const pesec_string_view_t name)
{
    const auto variable_node = (pesec_variable_node_t){.name = name};
    return pesec_node_new(
        PESEC_NODE_TYPE_VARIABLE,
        (pesec_node_data_t){
            .variable_node = variable_node
        }
    );
}

pesec_value_t pesec_variable_node_evaluate(const pesec_variable_node_t* variable_node, const pesec_environment_t* environment)
{
    return pesec_environment_get_variable(
        environment,
        variable_node->name
    )->value;
}

void pesec_variable_node_destroy(const pesec_variable_node_t* variable_node)
{
}
