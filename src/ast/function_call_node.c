#include "include/ast/function_call_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/array_value.h"
#include "include/ast/ast_node.h"
#include "include/function_value.h"
#include "include/module_value.h"
#include "include/structure_value.h"
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
    if (function_call_node->callee)
        ast_node_free(function_call_node->callee);

    function_call_argument_node_t *current = function_call_node->arguments;
    while (current)
    {
        if (current->value_expr)
            ast_node_free(current->value_expr);

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

    const function_value_t *function = callee_value.data.as_function;
    const ull_t total_arguments = function_call_node->arguments_count;

    value_t* eval_values = nullptr;
    string_view_t* arg_names = nullptr;

    if (total_arguments > 0)
    {
        eval_values = (value_t*)calloc(total_arguments, sizeof(value_t));
        arg_names = (string_view_t*)calloc(total_arguments, sizeof(string_view_t));
    }

    function_call_argument_node_t *current_arg = function_call_node->arguments;
    for (ull_t i = 0; i < total_arguments && current_arg; i++, current_arg = current_arg->next)
    {
        arg_names[i] = current_arg->name;
        eval_values[i] = ast_node_evaluate(current_arg->value_expr, context);
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


    const parameter_node_t *param = function->parameter->parameters;
    ull_t position_index = 0;

    while (param)
    {
        if (param->type == PARAMETER_NODE_TYPE_NORMAL)
        {
            value_t value = {0};
            bool found = false;

            if (position_index < total_arguments && arg_names[position_index].length == 0)
            {
                value = eval_values[position_index];
                position_index++;
                found = true;
            }
            else
            {
                for (ull_t i = 0; i < total_arguments; i++)
                {
                    if (arg_names[i].length > 0 && string_view_equals(arg_names[i], param->value))
                    {
                        value = eval_values[i];
                        arg_names[i].length = 0;
                        found = true;
                        break;
                    }
                }
            }

            if (!found) THROW("Missing required argument for parameter '%.*s'\n", (int)param->value.length, param->value.data);

            context_push(local_context, param->value, value, false);
        }
        else if (param->type == PARAMETER_NODE_TYPE_ARGS)
        {
            ull_t remaining = 0;
            for (ull_t i = position_index; i < total_arguments; i++)
                if (arg_names[i].length == 0) remaining++;

            value_t* arguments_array = nullptr;
            if (remaining > 0)
            {
                arguments_array = (value_t*)calloc(remaining, sizeof(value_t));
                ull_t idx = 0;
                for (ull_t i = position_index; i < total_arguments; i++)
                {
                    if (arg_names[i].length == 0)
                        arguments_array[idx++] = eval_values[i];
                }
            }

            context_push(local_context, param->value, value_new_array(array_value_new(arguments_array, remaining)), false);

            position_index = total_arguments;
        }
        else if (param->type == PARAMETER_NODE_TYPE_KWARGS)
        {
            context_t* kwarguments_context = context_new(nullptr);

            for (ull_t i = 0; i < total_arguments; i++)
            {
                if (arg_names[i].length > 0)
                    context_push(kwarguments_context, arg_names[i], eval_values[i], false);
            }

            context_push(local_context, param->value, value_new_structure(structure_value_new(kwarguments_context)), false);
        }
        param = param->next;
    }

    for (ull_t i = 0; i < total_arguments; i++)
    {
        if (arg_names[i].length > 0) THROW("Unexpected keyword argument '%.*s'\n", (int)arg_names[i].length, arg_names[i].data);
    }

    if (position_index < total_arguments) THROW("Too many positional arguments\n");

    value_t result = function_value_call(function, local_context);
    context_free(local_context);

    if (total_arguments > 0)
    {
        free(eval_values);
        free(arg_names);
    }

    return result;
}
