#ifndef PESEC_TOKEN_H
#define PESEC_TOKEN_H

#include "utils/string_view.h"
#include "string_value.h"
#include <stdio.h>

#include "number_value.h"

typedef enum
{
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_STRING,

    TOKEN_TYPE_KEYWORD,

    TOKEN_TYPE_EQUALS,
    TOKEN_TYPE_EQUALS_EQUALS,

    TOKEN_TYPE_NOT_EQUALS,

    TOKEN_TYPE_LESS,
    TOKEN_TYPE_LESS_EQUALS,

    TOKEN_TYPE_GREATER,
    TOKEN_TYPE_GREATER_EQUALS,

    TOKEN_TYPE_AMPERSAND,
    TOKEN_TYPE_AMPERSAND_AMPERSAND,

    TOKEN_TYPE_PIPE,
    TOKEN_TYPE_PIPE_PIPE,

    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_MINUS,
    TOKEN_TYPE_SLASH,
    TOKEN_TYPE_SLASH_SLASH,
    TOKEN_TYPE_ASTERISK,
    TOKEN_TYPE_ASTERISK_ASTERISK,

    TOKEN_TYPE_PLUS_EQUALS,
    TOKEN_TYPE_MINUS_EQUALS,
    TOKEN_TYPE_SLASH_EQUALS,
    TOKEN_TYPE_SLASH_SLASH_EQUALS,
    TOKEN_TYPE_ASTERISK_EQUALS,
    TOKEN_TYPE_ASTERISK_ASTERISK_EQUALS,

    TOKEN_TYPE_LPAREN,
    TOKEN_TYPE_RPAREN,

    TOKEN_TYPE_LBRACE,
    TOKEN_TYPE_RBRACE,

    TOKEN_TYPE_LBRACKET,
    TOKEN_TYPE_RBRACKET,

    TOKEN_TYPE_DOT,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_SEMICOLON,

    TOKEN_TYPE_AT_SIGN,

    TOKEN_TYPE_QUESTION_MARK,
    TOKEN_TYPE_EXCLAMATION_MARK,

    TOKEN_TYPE_EOF
} token_type_t;

typedef union
{
    string_view_t as_string_view;
    string_value_t* as_string;
    number_value_t* as_number;
} token_value_t;

typedef struct
{
    token_value_t value;
    token_type_t type;
    ull_t line;
} token_t;

void token_print(FILE* stream, token_t token);

const char* token_get_type(token_t token);

#endif // PESEC_TOKEN_H