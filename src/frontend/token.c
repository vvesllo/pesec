#include "include/frontend/token.h"

#include "include/common/panic.h"

pesec_token_t pesec_token_new(const pesec_token_type_t type, const pesec_token_data_t data)
{
    return (pesec_token_t){
        .type = type,
        .data = data
    };
}

pesec_token_t pesec_token_blank(const pesec_token_type_t type)
{
    return pesec_token_new(
        type,
        (pesec_token_data_t){
            .integer = 0
        }
    );
}

const char* pesec_token_type_to_cstr(const pesec_token_type_t type)
{
    switch (type)
    {
        case PESEC_TOKEN_EOF: return "<token:eof>";
        case PESEC_TOKEN_INTEGER: return "<token:integer>";
        case PESEC_TOKEN_FLOAT: return "<token:float>";
        case PESEC_TOKEN_IDENTIFIER: return "<token:identifier>";
        case PESEC_TOKEN_PLUS: return "<token:plus>";
        case PESEC_TOKEN_MINUS: return "<token:minus>";
        case PESEC_TOKEN_ASTERISK: return "<token:asterisk>";
        case PESEC_TOKEN_SLASH: return "<token:slash>";
        case PESEC_TOKEN_LPAREN: return "<token:left-paren>";
        case PESEC_TOKEN_RPAREN: return "<token:right-paren>";
        case PESEC_TOKEN_EQUAL: return "<token:equal>";
        case PESEC_TOKEN_COMMA: return "<token:comma>";
    }

    pesec_panic("token::to_cstr: unknown token type %d", type);
}
