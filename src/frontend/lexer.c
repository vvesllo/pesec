#include "include/frontend/lexer.h"

#include <ctype.h>

#include "include/common/panic.h"


static void pesec_lexer_advance(pesec_lexer_t* lexer)
{
    lexer->i += 1;
}

static bool pesec_lexer_can_advance(const pesec_lexer_t* lexer)
{
    return lexer->i < lexer->source.size;
}

static char pesec_lexer_peek(const pesec_lexer_t* lexer)
{
    return lexer->source.data[lexer->i];
}

static pesec_token_t pesec_lexer_next_numeric(pesec_lexer_t* lexer)
{
    double value = 0;
    while (pesec_lexer_can_advance(lexer) && isalnum(pesec_lexer_peek(lexer)))
    {
        value = value * 10 + pesec_lexer_peek(lexer) - '0';
        pesec_lexer_advance(lexer);
    }

    return pesec_token_new(
        PESEC_TOKEN_INTEGER,
        (pesec_token_data_t){
            .integer = (int64_t)value
        }
    );
}

static pesec_token_t pesec_lexer_next_identifier(pesec_lexer_t* lexer)
{
    const size_t begin = lexer->i;
    while (pesec_lexer_can_advance(lexer) && (isalnum(pesec_lexer_peek(lexer)) || pesec_lexer_peek(lexer) == '_'))
    {
        pesec_lexer_advance(lexer);
    }

    const auto value = (pesec_string_view_t){
        .data = lexer->source.data + begin,
        .size = lexer->i - begin
    };

    return pesec_token_new(
        PESEC_TOKEN_IDENTIFIER,
        (pesec_token_data_t){
            .string_view = value
        }
    );
}

static pesec_token_t pesec_lexer_next_operator(pesec_lexer_t* lexer)
{
    char current = pesec_lexer_peek(lexer);
    switch (current)
    {
        case '+': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_PLUS);
        case '-': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_MINUS);
        case '*': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_ASTERISK);
        case '/': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_SLASH);
        case '=': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_EQUAL);
        case ',': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_COMMA);
        case '(': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_LPAREN);
        case ')': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_RPAREN);
        default: pesec_panic("lexer::next_operator: unknown character `%c`", current);
    }
}

pesec_lexer_t pesec_lexer_new(const pesec_string_view_t source)
{
    return (pesec_lexer_t){
        .source = source,
        .i = 0
    };
}

pesec_token_t pesec_lexer_next(pesec_lexer_t* lexer)
{
    while (pesec_lexer_can_advance(lexer) && isspace(pesec_lexer_peek(lexer)))
        pesec_lexer_advance(lexer);

    if (!pesec_lexer_can_advance(lexer))
        return pesec_token_blank(PESEC_TOKEN_EOF);

    const char current = pesec_lexer_peek(lexer);
    if (isdigit(current)) return pesec_lexer_next_numeric(lexer);
    if (isalpha(current)) return pesec_lexer_next_identifier(lexer);
    return pesec_lexer_next_operator(lexer);
}
