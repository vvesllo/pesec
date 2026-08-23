#include "include/ast/import_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/utils/throw.h"

#include "include/lexer.h"
#include "include/module_value.h"
#include "include/parser.h"
#include "include/structure_value.h"
#include "include/utils/execute_file.h"

ast_node_t* import_node_new(ast_node_t* source)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));

    node->type = AST_NODE_IMPORT;
    node->node._import = (import_node_t*)malloc(sizeof(import_node_t));
    node->node._import->source = source;

    return node;
}

void import_node_free(import_node_t* import_node)
{
    ast_node_free(import_node->source);
    free(import_node);
}

value_t import_node_evaluate(const import_node_t* import_node, context_t* context)
{
    const value_t source_value = ast_node_evaluate(import_node->source, context);

    if (source_value.type != VALUE_TYPE_STRING)
        THROW("Import path should be a string");

    context_t* module_context = context_new(nullptr);

    execute_file(source_value.data.as_string->data, module_context);

    return MAKE_VAL_MODULE(module_value_new(module_context));
}
