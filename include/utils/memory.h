#ifndef PESEC_MEMORY_H
#define PESEC_MEMORY_H
#include "string_view.h"
#include "typedefs.h"

typedef enum MEMORY_VALUE_TYPE_ENUM
{
    MEMORY_VALUE_TYPE_STRING_VIEW,
    MEMORY_VALUE_TYPE_CHAR
} memory_value_type_t;

typedef union MEMORY_VALUE_VALUE_UNION
{
    string_view_t* as_string_view;
    char* as_char;
} memory_value_value_t;

typedef struct MEMORY_VALUE_STRUCT
{
    memory_value_value_t value;
    memory_value_type_t type;
} memory_value_t;

typedef struct MEMORY_STRUCT
{
    ull_t capacity;
    ull_t size;
    memory_value_t* values;
} memory_t;

memory_t* memory_get();

memory_value_t* memory_push(memory_t* memory, memory_value_t value);

void memory_free(memory_t* memory);

#endif // PESEC_MEMORY_H