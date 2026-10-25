#ifndef PESEC_TOKEN_H
#define PESEC_TOKEN_H

#include <inttypes.h>

#include "include/common/string_view.h"

typedef enum
{
    PESEC_TOKEN_EOF = 0,

    PESEC_TOKEN_NUMBER,
    PESEC_TOKEN_IDENTIFIER,
    PESEC_TOKEN_STRING,

    PESEC_TOKEN_EQUAL,

    PESEC_TOKEN_PLUS,
    PESEC_TOKEN_MINUS,
    PESEC_TOKEN_ASTERISK,
    PESEC_TOKEN_SLASH,
    PESEC_TOKEN_PERCENT,

    PESEC_TOKEN_COMMA,
    PESEC_TOKEN_DOT,
    PESEC_TOKEN_DOT_DOT,
    PESEC_TOKEN_DOT_DOT_DOT,

    PESEC_TOKEN_SEMICOLON,
    PESEC_TOKEN_COLON,

    PESEC_TOKEN_QUESTION_MARK,

    PESEC_TOKEN_EQUAL_EQUAL,

    PESEC_TOKEN_LESS_THAN,
    PESEC_TOKEN_LESS_EQUAL,

    PESEC_TOKEN_GREATER_THAN,
    PESEC_TOKEN_GREATER_EQUAL,

    PESEC_TOKEN_LPAREN,
    PESEC_TOKEN_RPAREN,

    PESEC_TOKEN_LBRACE,
    PESEC_TOKEN_RBRACE,

    PESEC_TOKEN_LBRACKET,
    PESEC_TOKEN_RBRACKET,

    PESEC_TOKEN_KEYWORD_IMMUT,
    PESEC_TOKEN_KEYWORD_MUT,
    PESEC_TOKEN_KEYWORD_FN,
    PESEC_TOKEN_KEYWORD_IF,
    PESEC_TOKEN_KEYWORD_ELSE,
    PESEC_TOKEN_KEYWORD_OBJECT,
    PESEC_TOKEN_KEYWORD_VECTOR,
    PESEC_TOKEN_KEYWORD_WHILE,
    PESEC_TOKEN_KEYWORD_FOR,
    PESEC_TOKEN_KEYWORD_IN,
    PESEC_TOKEN_KEYWORD_CONTINUE,
    PESEC_TOKEN_KEYWORD_BREAK,
    PESEC_TOKEN_KEYWORD_RETURN,
} pesec_token_type_t;

typedef union
{
    double number;
    pesec_string_view_t string_view;
} pesec_token_data_t;

typedef struct
{
    pesec_token_type_t type;
    pesec_token_data_t data;
    size_t line;
    size_t column;
} pesec_token_t;

pesec_token_t pesec_token_new(pesec_token_type_t type, pesec_token_data_t data, size_t line, size_t column);

pesec_token_t pesec_token_blank(pesec_token_type_t type, size_t line, size_t column);

const char* pesec_token_type_to_cstr(pesec_token_type_t type);

#endif // PESEC_TOKEN_H