#include "include/ast/variable_field_access_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/module_value.h"
#include "include/structure_value.h"
#include "include/utils/throw.h"


ast_node_t* variable_field_access_node_new(ast_node_t* object, const string_view_t field)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VARIABLE_FIELD_ACCESS;
    node->node.variable_field_access = (variable_field_access_node_t*)malloc(sizeof(variable_field_access_node_t));
    node->node.variable_field_access->object = object;
    node->node.variable_field_access->field = field;
    return node;
}

void variable_field_access_node_free(variable_field_access_node_t* variable_field_node)
{
    free(variable_field_node);
}

value_t variable_field_access_node_evaluate(const variable_field_access_node_t* variable_field_node, context_t* context)
{
    const value_t item = ast_node_evaluate(variable_field_node->object, context);

    switch (item.type)
    {
    case VALUE_TYPE_STRUCTURE:
        const structure_value_t* structure_value = item.data.as_structure;
        return structure_value_get(structure_value, variable_field_node->field);
    case VALUE_TYPE_MODULE:
        const module_value_t* module_value = item.data.as_module;
        return module_value_get(module_value, variable_field_node->field);
    default:
        THROW("Variable don't have fields");
    }
}
