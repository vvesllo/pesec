#include "include/context.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/array_value.h"
#include "include/function_value.h"
#include "include/utils/throw.h"

context_t* context_new(context_t* parent)
{
    const auto context = (context_t*)malloc(sizeof(context_t));

    context->capacity = 1024;
    context->size = 0;
    context->items = (context_item_t**)calloc(context->capacity,sizeof(context_item_t*));
    context->parent = parent;

    return context;
}

ull_t context_hash(const context_t* context, const string_view_t key)
{
    unsigned long long sum = 0;
    for (unsigned long long i = 0; i < key.length; i++)
    {
        sum += (key.data[i] * (i + 256)) ^ 'f' ^ 'u' ^ 'c' ^ 'k';
    }
    return sum % context->capacity;
}

void context_push(context_t* context, const string_view_t key, value_t value, const bool constant)
{
    const ull_t hash_index = context_hash(context, key);

    const auto item = (context_item_t*)malloc(sizeof(context_item_t));
    item->key = key;
    value_increase_reference(&value);
    item->value = value;
    item->constant = constant;
    item->next = nullptr;

    context_item_t* node = context->items[hash_index];

    if (node)
    {
        while (node->next)
        {
            node = node->next;
        }
        node->next = item;
    }
    else context->items[hash_index] = item;

    ++context->size;
}

void context_set(const context_t* context, const string_view_t key, value_t value)
{
    context_item_t* node = context_get(context, key);
    if (node->constant)
        THROW("Variable '%.*s' is constant\n", (unsigned int)key.length, key.data);

    value_increase_reference(&value);
    value_decrease_reference(&node->value);

    node->value = value;
}

context_item_t* context_get(const context_t* context, const string_view_t key)
{
    const ull_t hash_index = context_hash(context, key);

    context_item_t* node = context->items[hash_index];

    while (node)
    {
        if (node->key.length == key.length && string_view_equals(node->key, key))
            return node;

        node = node->next;
    }

    context_get_local(context, key);

    if (context->parent)
    {
        return context_get(context->parent, key);
    }

    THROW("Variable %.*s doesn't exist\n", (unsigned int)key.length, key.data);
}

context_item_t* context_get_local(const context_t* context, string_view_t key)
{
    const ull_t hash_index = context_hash(context, key);

    context_item_t* node = context->items[hash_index];

    while (node)
    {
        if (node->key.length == key.length && string_view_equals(node->key, key))
            return node;

        node = node->next;
    }

    THROW("Variable %.*s doesn't exist\n", (unsigned int)key.length, key.data);
}

void context_free(context_t* context)
{
    for (ull_t i = 0; i < context->capacity; ++i)
    {
        context_item_t* node = context->items[i];

        while (node)
        {
            context_item_t* next = node->next;
            value_decrease_reference(&node->value);
            free(node);
            node = next;
        }
    }

    free(context->items);
    free(context);
}