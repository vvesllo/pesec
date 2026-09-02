#include "include/ast/function_call_node.h"
#include <stdio.h>
#include <stdlib.h>

#include "include/array_value.h"
#include "include/ast/ast_node.h"
#include "include/function_value.h"
#include "include/module_value.h"
#include "include/utils/throw.h"

ast_node_t *function_call_node_new(ast_node_t *callee, function_call_argument_node_t *arguments, const ull_t count)
{
    const auto node = (ast_node_t *) malloc(sizeof(ast_node_t));

    node->type = AST_NODE_FUNCTION_CALL;
    node->node.function_call = (function_call_node_t *) malloc(sizeof(function_call_node_t));
    node->node.function_call->callee = callee;
    node->node.function_call->arguments = arguments;
    node->node.function_call->arguments_count = count;

    return node;
}

void function_call_node_free(function_call_node_t *function_call_node)
{
    if (function_call_node->callee) ast_node_free(function_call_node->callee);

    function_call_argument_node_t *current = function_call_node->arguments;
    while (current)
    {
        if (current->value_expr) ast_node_free(current->value_expr);

        function_call_argument_node_t *next = current->next;
        free(current);
        current = next;
    }

    free(function_call_node);
}

value_t function_call_node_evaluate(const function_call_node_t *function_call_node, context_t *context)
{
    const value_t callee_value = ast_node_evaluate(function_call_node->callee, context);

    if (callee_value.type != VALUE_TYPE_FUNCTION) THROW("Value typed %s is not callable\n", value_get_type(callee_value));

    const function_value_t* function = callee_value.data.as_function;
    const ull_t total_arguments = function_call_node->arguments_count;

    value_t* eval_values = nullptr;
    if (total_arguments > 0)
    {
        eval_values = (value_t*)calloc(total_arguments, sizeof(value_t));
        const function_call_argument_node_t* current_arg = function_call_node->arguments;
        for (ull_t i = 0; i < total_arguments && current_arg; i++, current_arg = current_arg->next)
        {
            eval_values[i] = ast_node_evaluate(current_arg->value_expr, context);
        }
    }

    context_t *local_context = nullptr;

    if (function_call_node->callee->type == AST_NODE_VARIABLE_FIELD_ACCESS)
    {
        const value_t object_value = ast_node_evaluate(function_call_node->callee->node.variable_field_access->object, context);
        if (object_value.type == VALUE_TYPE_MODULE)
            local_context = context_new(object_value.data.as_module->context);
        else
        {
            local_context = context_new(function->parent_context);
            if (object_value.type == VALUE_TYPE_STRUCTURE)
                context_push(local_context, string_view_from("this"), object_value, true);
        }
    }
    else
        local_context = context_new(function->parent_context);

    const parameter_node_t *parameter = function->parameter->parameters;
    ull_t position_index = 0;

    while (parameter)
    {
        if (parameter->type == PARAMETER_NODE_TYPE_NORMAL)
        {
            if (position_index >= total_arguments)
                THROW("Missing required argument for parameter '%.*s'\n",
                      (int)parameter->value.length, parameter->value.data);

            context_push(local_context, parameter->value, eval_values[position_index], false);
            position_index++;
        }
        else if (parameter->type == PARAMETER_NODE_TYPE_ARGS)
        {
            const ull_t remaining = total_arguments - position_index;

            value_t* arguments_array = nullptr;
            if (remaining > 0)
            {
                arguments_array = (value_t*)calloc(remaining, sizeof(value_t));
                for (ull_t i = 0; i < remaining; i++)
                {
                    arguments_array[i] = eval_values[position_index + i];
                }
            }

            context_push(local_context, parameter->value, value_new_array(array_value_new(arguments_array, remaining)), false);

            position_index = total_arguments;
        }

        parameter = parameter->next;
    }

    if (position_index < total_arguments) THROW("Too many positional arguments\n");

    const value_t result = function_value_call(function, local_context);
    context_free(local_context);

    if (total_arguments > 0)
        free(eval_values);

    if (result.control_flow == CONTROL_FLOW_RETURN)
        return result;

    return value_new_null();
}