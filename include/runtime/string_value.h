#ifndef PESEC_STRING_VALUE_H
#define PESEC_STRING_VALUE_H
#include "function_value.h"

constexpr size_t PESEC_STRING_VALUE_ITEMS_INLINE_SIZE = 256;

typedef struct PESEC_STRING_VALUE_STRUCT
{
    bool is_heap;
    size_t capacity;
    size_t size;

    union
    {
        char* heap;
        char _inline[PESEC_STRING_VALUE_ITEMS_INLINE_SIZE];
    } items;
} pesec_string_value_t;

pesec_string_value_t* pesec_string_value_new();

pesec_string_value_t* pesec_string_value_from(pesec_string_view_t string_view);

char* pesec_string_value_get_items(pesec_string_value_t* string_value);

const char* pesec_string_value_get_items_const(const pesec_string_value_t* string_value);

void pesec_string_value_push(pesec_string_value_t* string_value, char c);

void pesec_string_value_append(pesec_string_value_t* string_value, pesec_string_view_t string_view);

void pesec_string_value_destroy(pesec_string_value_t* string_value);

void pesec_string_value_print(FILE* stream, const pesec_string_value_t* string_value);


bool pesec_string_value_is_truthy(const pesec_string_value_t* string_value);

pesec_value_t pesec_string_value_negative(const pesec_string_value_t* string_value);

pesec_value_t pesec_string_value_add(const pesec_string_value_t* left, pesec_value_t right);

pesec_value_t pesec_string_value_sub(const pesec_string_value_t* left, pesec_value_t right);

pesec_value_t pesec_string_value_mul(const pesec_string_value_t* left, pesec_value_t right);

pesec_value_t pesec_string_value_div(const pesec_string_value_t* left, pesec_value_t right);

pesec_value_t pesec_string_value_mod(const pesec_string_value_t* left, pesec_value_t right);

bool pesec_string_value_equal(const pesec_string_value_t* left, pesec_value_t right);

bool pesec_string_value_less(const pesec_string_value_t* left, pesec_value_t right);

bool pesec_string_value_less_equal(const pesec_string_value_t* left, pesec_value_t right);

bool pesec_string_value_greater(const pesec_string_value_t* left, pesec_value_t right);

bool pesec_string_value_greater_equal(const pesec_string_value_t* left, pesec_value_t right);


pesec_value_t pesec_string_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_STRING_VALUE_H
