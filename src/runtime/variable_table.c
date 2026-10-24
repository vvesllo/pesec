#include "include/runtime/variable_table.h"

#include <string.h>

#include "include/common/alloc.h"
#include "include/common/panic.h"


static size_t pesec_variable_table_index(const uint64_t hash, const size_t capacity)
{
    return hash & (capacity - 1);
}

static size_t pesec_variable_table_hash(const pesec_variable_table_t* variable_table, const pesec_string_view_t name)
{
    size_t hash = 0xcbf29ce484222325ULL;
    size_t i = 0;
    for (; i + 8 <= name.size; i += 8)
    {
        size_t chunk;
        memcpy(&chunk, name.data + i, 8);
        hash ^= chunk;
        hash *= 0x100000001b3ULL;
    }

    for (; i < name.size; i++)
    {
        hash ^= (unsigned char)name.data[i];
        hash *= 0x100000001b3ULL;
    }

    return pesec_variable_table_index(hash, variable_table->capacity);
}

static pesec_variable_t* pesec_variable_table_get_items(pesec_variable_table_t* variable_table)
{
    return variable_table->is_heap ? variable_table->items.heap : variable_table->items._inline;
}

static bool* pesec_variable_table_get_occupied(pesec_variable_table_t* variable_table)
{
    return variable_table->is_heap ? variable_table->occupied.heap : variable_table->occupied._inline;
}

static const pesec_variable_t* pesec_variable_table_get_items_const(const pesec_variable_table_t* variable_table)
{
    return variable_table->is_heap ? variable_table->items.heap : variable_table->items._inline;
}

static const bool* pesec_variable_table_get_occupied_const(const pesec_variable_table_t* variable_table)
{
    return variable_table->is_heap ? variable_table->occupied.heap : variable_table->occupied._inline;
}

static void pesec_variable_table_rehash(pesec_variable_table_t* variable_table)
{
    const size_t new_capacity = variable_table->capacity << 1;

    const auto new_items = (pesec_variable_t*)pesec_malloc(new_capacity * sizeof(pesec_variable_t));
    const auto new_occupied = (bool*)pesec_malloc(new_capacity * sizeof(bool));

    memset(new_occupied, 0, new_capacity * sizeof(bool));

    const bool was_heap = variable_table->is_heap;
    pesec_variable_t* old_items = was_heap ? variable_table->items.heap : variable_table->items._inline;
    bool* old_occupied = was_heap ? variable_table->occupied.heap : variable_table->occupied._inline;
    const size_t old_capacity = variable_table->capacity;
    const size_t old_size = variable_table->size;

    variable_table->capacity = new_capacity;

    size_t new_size = 0;
    for (size_t i = 0; i < old_capacity; i++)
    {
        if (!old_occupied[i])
            continue;

        const size_t hash = pesec_variable_table_hash(variable_table, old_items[i].name);

        for (size_t probe = 0; probe < new_capacity; probe++)
        {
            const size_t index = (hash + probe) & (new_capacity - 1);
            if (!new_occupied[index])
            {
                new_occupied[index] = true;
                new_items[index] = old_items[i];
                new_size += 1;
                break;
            }
        }
    }

    if (new_size != old_size)
        pesec_panic("variable_table::rehash: size mismatch after rehash");

    if (was_heap)
    {
        pesec_free(old_items);
        pesec_free(old_occupied);
    }

    variable_table->items.heap = new_items;
    variable_table->occupied.heap = new_occupied;
    variable_table->is_heap = true;
    variable_table->size = new_size;
}

pesec_variable_table_t* pesec_variable_table_new()
{
    const auto variable_table = (pesec_variable_table_t*)pesec_malloc(sizeof(pesec_variable_table_t));

    variable_table->is_heap = false;
    variable_table->size = 0;
    variable_table->capacity = PESEC_VARIABLE_TABLE_ITEMS_INLINE_SIZE;
    memset(variable_table->occupied._inline, 0, sizeof(variable_table->occupied._inline));

    return variable_table;
}

void pesec_variable_table_reset(pesec_variable_table_t* variable_table)
{
    bool* occupied = pesec_variable_table_get_occupied(variable_table);
    pesec_variable_t* items = pesec_variable_table_get_items(variable_table);

    for (size_t i = 0; i < variable_table->capacity; i++)
    {
        if (occupied[i])
        {
            pesec_value_decrease_reference(&items[i].value);
            occupied[i] = false;
        }
    }

    variable_table->size = 0;
}

void pesec_variable_table_push(pesec_variable_table_t* variable_table, const pesec_variable_t variable)
{
    if ((variable_table->size + 1) * 4 >= variable_table->capacity * 3)
        pesec_variable_table_rehash(variable_table);

    const size_t hash = pesec_variable_table_hash(variable_table, variable.name);

    pesec_variable_t* items = pesec_variable_table_get_items(variable_table);
    bool* occupied = pesec_variable_table_get_occupied(variable_table);

    for (size_t probe = 0; probe < variable_table->capacity; probe++)
    {
        const size_t index = pesec_variable_table_index(hash + probe, variable_table->capacity);
        if (!occupied[index])
        {
            occupied[index] = true;
            items[index] = variable;
            variable_table->size += 1;
            return;
        }

        if (pesec_string_view_equals(items[index].name, variable.name))
            pesec_panic("variable_table::push: variable `%.*s` already exists",
                        variable.name.size, variable.name.data);
    }

    pesec_panic("variable_table::push: table full after rehash (should not happen)");
}

pesec_variable_t* pesec_variable_table_find(
    const pesec_variable_table_t* variable_table,
    const pesec_string_view_t name
)
{
    const size_t hash = pesec_variable_table_hash(variable_table, name);

    const pesec_variable_t* items = pesec_variable_table_get_items_const(variable_table);
    const bool* occupied = pesec_variable_table_get_occupied_const(variable_table);

    for (size_t probe = 0; probe < variable_table->capacity; probe++)
    {
        const size_t index = pesec_variable_table_index(hash + probe, variable_table->capacity);
        if (!occupied[index])
            return nullptr;

        if (pesec_string_view_equals(items[index].name, name))
            return (pesec_variable_t*)&items[index];
    }

    return nullptr;
}

void pesec_variable_table_destroy(pesec_variable_table_t* variable_table)
{
    bool* occupied = pesec_variable_table_get_occupied(variable_table);
    pesec_variable_t* items = pesec_variable_table_get_items(variable_table);
    for (size_t i = 0; i < variable_table->capacity; i++)
    {
        if (occupied[i])
            pesec_value_decrease_reference(&items[i].value);
    }

    if (variable_table->is_heap)
    {
        pesec_free(occupied);
        pesec_free(items);
    }

    pesec_free(variable_table);
}
