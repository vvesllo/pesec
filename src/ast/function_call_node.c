#include "include/ast/function_call_node.h"

#include <stdio.h>

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/function_value.h"
#include "include/module_value.h"
#include "include/structure_value.h"
#include "include/utils/throw.h"


ast_node_t* function_call_node_new(ast_node_t* callee, ast_node_t* arguments)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_FUNCTION_CALL;
    node->node.function_call = (function_call_node_t*)malloc(sizeof(function_call_node_t));
    node->node.function_call->callee = callee;
    node->node.function_call->arguments = arguments;
    return node;
}

void function_call_node_free(function_call_node_t* function_call_node)
{
    if (function_call_node->arguments)
        ast_node_free(function_call_node->arguments);
    if (function_call_node->callee)
        ast_node_free(function_call_node->callee);
    free(function_call_node);
}

value_t function_call_node_evaluate(const function_call_node_t* function_call_node, context_t* context)
{
    const value_t callee_value = ast_node_evaluate(function_call_node->callee, context);

    if (callee_value.type != VALUE_TYPE_FUNCTION)
    {
        THROW(
            "Value typed %s is not callable\n",
            value_get_type(callee_value)
            );
    }

    const auto function = callee_value.data.as_function;

    const ull_t statements_count = function_call_node->arguments->node.statement_sequence->count;
    const statement_sequence_node_queue_t* current = nullptr;
    value_t* evaluated_values = nullptr;

    auto result = MAKE_VAL_NUM(0);

    if (statements_count > 0)
    {
        current = function_call_node->arguments->node.statement_sequence->statements;
        evaluated_values = (value_t*)calloc(statements_count, sizeof(value_t));
        if (!evaluated_values) return MAKE_VAL_NUM(0);
    }

    for (ull_t i = 0; current && i < statements_count; i++, current = current->next)
    {
        if (current->statement)
            evaluated_values[i] = ast_node_evaluate(current->statement, context);
        else
            evaluated_values[i] = MAKE_VAL_NUM(0);
    }

    if (function->parameter->count != statements_count)
    {
        THROW(
            "Function takes %llu but %llu given\n",
            function->parameter->count, statements_count
            );
    }

    context_t* local_context = nullptr;

    if (function_call_node->callee->type == AST_NODE_VARIABLE_FIELD_ACCESS)
    {
        const value_t object_value = ast_node_evaluate(function_call_node->callee->node.variable_field_access->object, context);

        if (object_value.type == VALUE_TYPE_MODULE)
            local_context = context_new(object_value.data.as_module->context);
        else
        {
            local_context = context_new(context);
            if (object_value.type == VALUE_TYPE_STRUCTURE)
                context_push(local_context, string_view_from("this"), object_value, true);
        }
    }
    else
        local_context = context_new(context);

    const parameter_queue_t* parameter = function->parameter->parameters;
    for (ull_t i = 0; parameter; ++i, parameter = parameter->next)
    {
        context_push(local_context, parameter->value, evaluated_values[i], false);
    }

    result = function_value_call(function, local_context);

    context_free(local_context);

    if (evaluated_values) free(evaluated_values);

    return result;
}
