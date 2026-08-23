#include "include/parameter.h"

#include <stdio.h>
#include <stdlib.h>


parameter_t* parameter_new()
{
    const auto parameter = (parameter_t*)malloc(sizeof(parameter_t));
    parameter->count = 0;
    parameter->parameters = nullptr;
    return parameter;
}

parameter_t* parameter_new_from_cstr(const char** values)
{
    parameter_t* parameter = parameter_new();

    for (const char **p = values; *p; ++p)
    {
        parameter_push(parameter, string_view_from(*p));
    }

    return parameter;
}

void parameter_push_from_cstr(parameter_t* parameter, const char** values)
{
    for (const char **p = values; *p; ++p)
    {
        parameter_push(parameter, string_view_from(*p));
    }
}

void parameter_push(parameter_t* parameter, const string_view_t value)
{
    ++parameter->count;

    const auto new_node = (parameter_queue_t*)malloc(sizeof(parameter_queue_t));

    new_node->value = value;
    new_node->next = nullptr;

    if (parameter->parameters == nullptr)
    {
        parameter->parameters = new_node;
        return;
    }

    parameter_queue_t* current = parameter->parameters;


    // TODO: сделать указатель на последний элемент и заменить эту хуйню
    while (current->next)
    {
        current = current->next;
    }

    current->next = new_node;
}

void parameter_free(parameter_t* parameter)
{
    parameter_queue_t* current = parameter->parameters;

    while (current)
    {
        parameter_queue_t* next = current->next;
        free(current);
        current = next;
    }

    free(parameter);
}