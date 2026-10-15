#include "include/common/string_view.h"

#include <string.h>

pesec_string_view_t pesec_string_view_new(const char* data, const size_t size)
{
    return (pesec_string_view_t){
        .data = data,
        .size = size
    };
}

pesec_string_view_t pesec_string_view_from(const char* cstr)
{
    return (pesec_string_view_t){
        .data = cstr,
        .size = strlen(cstr)
    };
}

bool pesec_string_view_equals(const pesec_string_view_t left, const pesec_string_view_t right)
{
    return left.size == right.size && memcmp(left.data, right.data, left.size) == 0;
}

bool pesec_string_view_equals_cstr(const pesec_string_view_t left, const char* right)
{
    return left.size == strlen(right) && memcmp(left.data, right, left.size) == 0;
}
