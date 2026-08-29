#ifndef PESEC_PARAMETER_H
#define PESEC_PARAMETER_H

#include "include/utils/string_view.h"
#include "utils/typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct PARAMETER_NODE_STRUCT parameter_node_t;

typedef enum
{
    PARAMETER_NODE_TYPE_NORMAL,
    PARAMETER_NODE_TYPE_ARGS,
    PARAMETER_NODE_TYPE_KWARGS,
} parameter_node_type_t;

typedef struct PARAMETER_NODE_STRUCT
{
    parameter_node_type_t type;
    string_view_t value;
    parameter_node_t* next;
} parameter_node_t;

typedef struct
{
    ull_t count;
    parameter_node_t* parameters;
} parameter_t;

parameter_t* parameter_new();

parameter_t* parameter_new_from_cstr(const char** values);

void parameter_push_from_cstr(parameter_t* parameter, const char** values);

void parameter_push(parameter_t* parameter, string_view_t value, parameter_node_type_t type);

void parameter_free(parameter_t* parameter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_PARAMETER_H