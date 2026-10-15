#include "include/common/alloc.h"

#include <stdlib.h>

#include "include/common/panic.h"

void* pesec_malloc(const size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        pesec_panic("pesec::malloc: out of memory");
    return ptr;
}

void* pesec_calloc(const size_t count, const size_t size)
{
    void* ptr = calloc(count, size);
    if (!ptr)
        pesec_panic("pesec::calloc: out of memory");
    return ptr;
}

void* pesec_realloc(void* ptr, const size_t size)
{
    void* temp_ptr = realloc(ptr, size);
    if (!temp_ptr)
        pesec_panic("pesec::realloc: out of memory");
    return temp_ptr;
}

void pesec_free(void* ptr)
{
    if (!ptr)
        pesec_panic("pesec::free: memory is null");
    free(ptr);
}
