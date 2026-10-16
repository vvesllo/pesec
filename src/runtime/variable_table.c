#include "include/runtime/variable_table.h"

#include "include/common/alloc.h"
#include "include/common/panic.h"


static size_t pesec_variable_table_hash(const pesec_variable_table_t* variable_table, const pesec_string_view_t name)
{
    size_t hash = 5381;
    for (size_t i = 0; i < name.size; i++)
    {
        hash += (hash << 5) + (unsigned char)name.data[i];
        hash &= 0xFFFFFFFF;
    }

    return hash % variable_table->capacity;
}

pesec_variable_table_t pesec_variable_table_new()
{
    pesec_variable_table_t variable_table;

    variable_table.size = 0;
    variable_table.capacity = 1024;
    variable_table.items = (pesec_variable_t*)pesec_calloc(variable_table.capacity, sizeof(pesec_variable_t));
    variable_table.occupied = (bool*)pesec_calloc(variable_table.capacity, sizeof(bool));

    return variable_table;
}

void pesec_variable_table_push(pesec_variable_table_t* variable_table, const pesec_variable_t variable)
{
    const size_t hash = pesec_variable_table_hash(variable_table, variable.name);

    for (size_t probe = 0; probe < variable_table->capacity; probe++)
    {
        const size_t index = (hash + probe) % variable_table->capacity;

        if (pesec_string_view_equals(variable_table->items[index].name, variable.name))
            pesec_panic("variable_table::push: variable `%.*s` already exists",
                        variable.name.size, variable.name.data
            );

        if (!variable_table->occupied[index])
        {
            variable_table->occupied[index] = true;
            variable_table->items[index] = variable;
            variable_table->size += 1;
            break;
        }
    }
}

void pesec_variable_table_set(const pesec_variable_table_t* variable_table, const pesec_string_view_t name, const pesec_value_t value)
{
    const size_t hash = pesec_variable_table_hash(variable_table, name);

    for (size_t probe = 0; probe < variable_table->capacity; probe++)
    {
        const size_t index = (hash + probe) % variable_table->capacity;

        if (!variable_table->occupied[index])
            continue;

        pesec_variable_t* variable = &variable_table->items[index];

        if (pesec_string_view_equals(variable->name, name))
        {
            if (variable->immutable)
                pesec_panic("variable_table::set: variable `%.*s` is immutable",
                            name.size, name.data
                );

            variable->value = value;
            return;
        }
    }

    pesec_panic("variable_table::set: variable `%.*s` doesn't exists",
                name.size, name.data
    );
}

pesec_variable_t* pesec_variable_table_get(const pesec_variable_table_t* variable_table, const pesec_string_view_t name)
{
    const size_t hash = pesec_variable_table_hash(variable_table, name);

    for (size_t probe = 0; probe < variable_table->capacity; probe++)
    {
        const size_t index = (hash + probe) % variable_table->capacity;

        if (!variable_table->occupied[index])
            continue;

        pesec_variable_t* variable = &variable_table->items[index];

        if (pesec_string_view_equals(variable->name, name))
            return variable;
    }

    pesec_panic("variable_table::get: variable `%.*s` doesn't exists",
                name.size, name.data
    );
}

void pesec_variable_table_destroy(const pesec_variable_table_t* variable_table)
{
    pesec_free(variable_table->items);
    pesec_free(variable_table->occupied);
}
