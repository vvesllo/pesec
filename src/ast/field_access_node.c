#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/object_value.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_field_access_node_new(pesec_node_t* object, pesec_string_view_t field)
{
    const auto field_access_node = (pesec_field_access_node_t){
        .object = object,
        .field = field,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_FIELD_ACCESS,
        (pesec_node_data_t){
            .field_access_node = field_access_node
        }
    );
}

pesec_value_t pesec_field_access_node_evaluate(
    const pesec_field_access_node_t* field_access_node,
    pesec_environment_t* environment
)
{
    pesec_value_t value = pesec_node_evaluate(field_access_node->object, environment);

    switch (value.type)
    {
        case PESEC_VALUE_TYPE_VECTOR:
            return pesec_vector_value_method_handler(&value, field_access_node->field, environment);
        case PESEC_VALUE_TYPE_OBJECT:
            return pesec_object_value_method_handler(&value, field_access_node->field, environment);
        default:
            pesec_panic("field_access_node::evaluate: can't get field from non object value");
    }
}

void pesec_field_access_node_destroy(const pesec_field_access_node_t* field_access_node)
{
    pesec_node_destroy(field_access_node->object);
}
