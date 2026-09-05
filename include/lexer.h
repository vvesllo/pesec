#ifndef PESEC_LEXER_H
#define PESEC_LEXER_H

#include "token.h"

#define LEXER_NEW_TOKEN_FROM_CURRENT_POS(_offset, _length, _type) \
    (token_t) { .line = lexer->line, .value.as_string_view = (string_view_t) { .data = lexer->source + _offset, .length = _length }, .type = _type }

typedef struct
{
    bool commenting;
    char* source;
    ull_t i;
    ull_t length;
    ull_t line;
} lexer_t;

lexer_t* lexer_new(char* source, ull_t length);

void lexer_advance(lexer_t* lexer);

bool lexer_can_advance(const lexer_t* lexer);

char lexer_get_current_char(const lexer_t* lexer);

void lexer_skip_every_unnecessary_shit(lexer_t* lexer);

token_t lexer_next_token(lexer_t* lexer);

token_t lexer_next_number(lexer_t* lexer);

token_t lexer_next_identifier(lexer_t* lexer);

token_t lexer_next_string(lexer_t* lexer, char quote);

token_t lexer_next_operator(lexer_t* lexer);

void lexer_free(lexer_t* lexer);

#endif // PESEC_LEXER_H