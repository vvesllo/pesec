#include "include/ast/function_definition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/function_value.h"


ast_node_t* function_definition_node_new(parameter_t* parameter, ast_node_t* body)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_FUNCTION_DEFINITION;
    node->node.function_definition = (function_definition_node_t*)malloc(sizeof(function_definition_node_t));
    node->node.function_definition->parameter = parameter;
    node->node.function_definition->body = body;
    return node;
}

void function_definition_node_free(function_definition_node_t* function_definition_node)
{
    ast_node_free(function_definition_node->body);
    free(function_definition_node);
}

value_t function_definition_node_evaluate(const function_definition_node_t* function_definition_node, context_t* context)
{
    const auto value = value_new_function(
        function_value_new(
            function_definition_node->parameter,
            (function_value_value_t){
                .as_node=function_definition_node->body
            },
            FUNCTION_VALUE_TYPE_NODE,
            context
        )
    );
    return value;
}
