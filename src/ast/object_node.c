#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/object_value.h"

pesec_node_t* pesec_object_node_new(const pesec_node_vector_t node_vector)
{
    const auto object_node = (pesec_object_node_t){
        .node_vector = node_vector
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_OBJECT,
        (pesec_node_data_t){
            .object_node = object_node
        }
    );
}

pesec_value_t pesec_object_node_evaluate(const pesec_object_node_t* object_node, pesec_environment_t* environment)
{
    pesec_object_value_t* object_value = pesec_object_value_new();
    for (size_t i = 0; i < object_node->node_vector.size; i++)
    {
        const pesec_node_t* node = object_node->node_vector.items[i];

        if (node->type != PESEC_NODE_TYPE_DEFINITION)
            pesec_panic("object_node::evaluate: object can contain only definition nodes");

        const pesec_definition_node_t* definition = &node->data.definition_node;
        const pesec_value_t value = pesec_node_evaluate(definition->value, environment);

        pesec_object_value_push(object_value, definition->name, value, definition->immutable);
    }

    return pesec_value_as_object(object_value);
}

void pesec_object_node_destroy(const pesec_object_node_t* object_node)
{
    pesec_node_vector_destroy(&object_node->node_vector);
}
