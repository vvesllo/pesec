#include "include/utils/memory.h"

#include <stdlib.h>

#include "include/utils/throw.h"

memory_t* memory_get()
{
    static memory_t* memory = nullptr;
    if (!memory)
    {
        memory = (memory_t*)malloc(sizeof(memory_t));
        memory->capacity = 16;
        memory->size = 0;
        memory->values = (memory_value_t*)calloc(memory->capacity, sizeof(memory_value_t));
    }

    return memory;
}

memory_value_t* memory_push(memory_t* memory, const memory_value_t value)
{
    if (memory->size >= memory->capacity)
    {
        memory->capacity *= 2;
        const auto temp = (memory_value_t*)realloc(memory->values, sizeof(memory_value_t) * memory->capacity);
        if (!temp) THROW("Failed to allocate memory value array");
        memory->values = temp;
    }

    memory->size++;
    memory->values[memory->size - 1] = value;
    return &memory->values[memory->size - 1];
}

void memory_free(memory_t* memory)
{
    for (ull_t i = 0; i < memory->size; i++)
    {
        switch (memory->values[i].type)
        {
        case MEMORY_VALUE_TYPE_STRING_VIEW: if (memory->values[i].value.as_string_view) free(memory->values[i].value.as_string_view); break;
        case MEMORY_VALUE_TYPE_CHAR: if (memory->values[i].value.as_char) free(memory->values[i].value.as_char); break;
        default: THROW("Unknown memory value type %d\n", memory->values[i].type); break;
        }
    }

    free(memory);
}