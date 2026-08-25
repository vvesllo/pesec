#ifndef PESEC_STRING_VIEW_H
#define PESEC_STRING_VIEW_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct
{
    const char* data;
    unsigned long long length;
} string_view_t;

string_view_t string_view_from(const char* string);

bool string_view_equals(string_view_t left, string_view_t right);

bool string_view_equals_cstr(string_view_t left, const char* right);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_STRING_VIEW_H
