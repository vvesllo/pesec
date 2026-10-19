#ifndef PESEC_VARIABLE_TABLE_H
#define PESEC_VARIABLE_TABLE_H

#include "value.h"
#include "include/common/string_view.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;

typedef struct PESEC_VARIABLE_STRUCT
{
    pesec_string_view_t name;
    pesec_value_t value;
    bool immutable;
} pesec_variable_t;

typedef struct
{
    pesec_variable_t* items;
    bool* occupied;
    size_t size;
    size_t capacity;
} pesec_variable_table_t;

pesec_variable_table_t pesec_variable_table_new();

void pesec_variable_table_push(pesec_variable_table_t* variable_table, pesec_variable_t variable);

pesec_variable_t* pesec_variable_table_find(const pesec_variable_table_t* variable_table, pesec_string_view_t name);

void pesec_variable_table_destroy(const pesec_variable_table_t* variable_table);

#endif // PESEC_VARIABLE_TABLE_H
