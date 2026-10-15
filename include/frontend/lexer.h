#ifndef PESEC_LEXER_H
#define PESEC_LEXER_H

#include "include/common/string_view.h"
#include "include/frontend/token.h"

typedef struct
{
    pesec_string_view_t source;
    size_t i;
} pesec_lexer_t;

pesec_lexer_t pesec_lexer_new(pesec_string_view_t source);

pesec_token_t pesec_lexer_next(pesec_lexer_t* lexer);

#endif // PESEC_LEXER_H