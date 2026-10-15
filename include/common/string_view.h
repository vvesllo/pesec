#ifndef PESEC_STRING_VIEW_H
#define PESEC_STRING_VIEW_H

#include <stddef.h>

typedef struct
{
    const char* data;
    size_t size;
} pesec_string_view_t;

pesec_string_view_t pesec_string_view_new(const char* data, size_t size);

pesec_string_view_t pesec_string_view_from(const char* cstr);

bool pesec_string_view_equals(pesec_string_view_t left, pesec_string_view_t right);

bool pesec_string_view_equals_cstr(pesec_string_view_t left, const char* right);

#endif // PESEC_STRING_VIEW_H