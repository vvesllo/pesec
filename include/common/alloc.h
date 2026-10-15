#ifndef PESEC_ALLOC_H
#define PESEC_ALLOC_H

#include <stddef.h>

void* pesec_malloc(size_t size);

void* pesec_calloc(size_t count, size_t size);

void* pesec_realloc(void* ptr, size_t size);

void pesec_free(void* ptr);

#endif // PESEC_ALLOC_H