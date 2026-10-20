#include "include/common/alloc.h"

#include <stdlib.h>

#include "include/common/panic.h"

static struct
{
    void** items;
    size_t size;
    size_t capacity;
} g_memory = {
    .items = nullptr,
    .size = 0,
    .capacity = 0
};

static void pesec_memory_push(void* ptr)
{
    if (g_memory.size >= g_memory.capacity)
    {
        g_memory.capacity = g_memory.capacity ? g_memory.capacity << 1 : 64;
        const auto items = (void**)realloc(g_memory.items, g_memory.capacity * sizeof(void*));
        if (!items) pesec_panic("pesec::malloc: out of memory");
        g_memory.items = items;
    }
    g_memory.items[g_memory.size] = ptr;
    g_memory.size += 1;
}

static void pesec_memory_remove(const void* ptr)
{
    for (size_t i = 0; i < g_memory.size; i++)
    {
        if (g_memory.items[i] == ptr)
        {
            g_memory.items[i] = g_memory.items[g_memory.size - 1];
            g_memory.size -= 1;
            return;
        }
    }
}

void* pesec_malloc(const size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        pesec_panic("pesec::malloc: out of memory");
    pesec_memory_push(ptr);
    return ptr;
}

void* pesec_calloc(const size_t count, const size_t size)
{
    void* ptr = calloc(count, size);
    if (!ptr)
        pesec_panic("pesec::calloc: out of memory");
    pesec_memory_push(ptr);
    return ptr;
}

void* pesec_realloc(void* ptr, const size_t size)
{
    if (!ptr) return pesec_malloc(size);
    pesec_memory_remove(ptr);

    void* temp_ptr = realloc(ptr, size);
    if (!temp_ptr)
        pesec_panic("pesec::realloc: out of memory");

    pesec_memory_push(temp_ptr);
    return temp_ptr;
}

void pesec_free(void* ptr)
{
    if (!ptr) return;

    pesec_memory_remove(ptr);
    free(ptr);
}

void pesec_alloc_cleanup_all()
{
    for (size_t i = 0; i < g_memory.size; i++)
    {
        if (g_memory.items[i])
            free(g_memory.items[i]);
    }

    free(g_memory.items);

    g_memory.items = nullptr;
    g_memory.size = 0;
    g_memory.capacity = 0;
}
