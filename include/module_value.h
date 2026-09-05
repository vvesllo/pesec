#ifndef PESEC_MODULE_VALUE_H
#define PESEC_MODULE_VALUE_H

#include "context.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_STRUCT statement_sequence_node_t;

typedef struct MODULE_VALUE_STRUCT
{
    context_t* context;
} module_value_t;

module_value_t* module_value_new(context_t* context);

value_t module_value_get(const module_value_t* module_value, string_view_t name);

void module_value_set(const module_value_t* module_value, string_view_t name, value_t value);

void module_value_free(module_value_t* module_value);

value_t module_value_get_fields(const module_value_t* module_value);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_MODULE_VALUE_H
