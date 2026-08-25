#ifndef PESEC_PARAMETER_H
#define PESEC_PARAMETER_H

#include "include/utils/string_view.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct PARAMETER_QUEUE_STRUCT parameter_queue_t;

typedef struct PARAMETER_QUEUE_STRUCT
{
    string_view_t value;
    parameter_queue_t* next;
} parameter_queue_t;

typedef struct
{
    unsigned long long count;
    parameter_queue_t* parameters;
} parameter_t;

parameter_t* parameter_new();

parameter_t* parameter_new_from_cstr(const char** values);

void parameter_push_from_cstr(parameter_t* parameter, const char** values);

void parameter_push(parameter_t* parameter, string_view_t value);

void parameter_free(parameter_t* parameter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_PARAMETER_H