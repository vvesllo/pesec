#include "include/ast/node.h"
#include "include/runtime/environment.h"
#include "include/runtime/function_value.h"

pesec_node_t* pesec_function_node_new(pesec_node_t* body, pesec_parameter_vector_t parameter_vector)
{
    const auto function_node = (pesec_function_node_t){
        .body = body,
        .parameter_vector = parameter_vector,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_FUNCTION,
        (pesec_node_data_t){
            .function_node = function_node
        }
    );
}

pesec_value_t pesec_function_node_evaluate(
    const pesec_function_node_t* function_node,
    pesec_environment_t* environment
)
{
    return pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_NODE,
            (pesec_function_value_data_t){
                .node = function_node->body
            },
            function_node->parameter_vector,
            environment
        )
    );
}

void pesec_function_node_destroy(const pesec_function_node_t* function_node)
{
    pesec_node_destroy(function_node->body);
    pesec_parameter_vector_destroy(&function_node->parameter_vector);
}
