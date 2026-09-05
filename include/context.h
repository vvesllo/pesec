#ifndef PESEC_CONTEXT_H
#define PESEC_CONTEXT_H

#include "value.h"
#include "utils/string_view.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct CONTEXT_STRUCT context_t;
typedef struct CONTEXT_ITEM_STRUCT context_item_t;

typedef struct CONTEXT_KEYS_STRUCT
{
    string_view_t* keys;
    ull_t size;
    ull_t capacity;
} context_keys_t;

typedef struct CONTEXT_ITEM_STRUCT
{
    string_view_t key;
    value_t value;
    bool constant;
    context_item_t* next;
} context_item_t;

typedef struct CONTEXT_STRUCT
{
    context_item_t** items;
    ull_t size;
    ull_t capacity;
    context_t* parent;
    context_keys_t* keys;
} context_t;

context_t* context_new(context_t* parent);

ull_t context_hash(const context_t* context, string_view_t key);

void context_push(context_t* context, string_view_t key, value_t value, bool constant);

void context_set(const context_t* context, string_view_t key, value_t value);

context_item_t* context_get(const context_t* context, string_view_t key);

context_item_t* context_get_local(const context_t* context, string_view_t key);

void context_free(context_t* context);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_CONTEXT_H
