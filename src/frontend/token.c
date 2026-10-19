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
        case PESEC_TOKEN_EQUAL_EQUAL: return "<token:equal-equal>";
        case PESEC_TOKEN_LESS_THAN: return "<token:less-than>";
        case PESEC_TOKEN_LESS_EQUAL: return "<token:less-equal>";
        case PESEC_TOKEN_GREATER_THAN: return "<token:greater-than>";
        case PESEC_TOKEN_GREATER_EQUAL: return "<token:greater-equal>";
        case PESEC_TOKEN_LPAREN: return "<token:left-paren>";
        case PESEC_TOKEN_RPAREN: return "<token:right-paren>";
        case PESEC_TOKEN_LBRACE: return "<token:left-brace>";
        case PESEC_TOKEN_RBRACE: return "<token:right-brace>";
        case PESEC_TOKEN_LBRACKET: return "<token:left-bracket>";
        case PESEC_TOKEN_RBRACKET: return "<token:right-bracket>";
        case PESEC_TOKEN_EQUAL: return "<token:equal>";
        case PESEC_TOKEN_COMMA: return "<token:comma>";
        case PESEC_TOKEN_DOT: return "<token:dot>";
        case PESEC_TOKEN_SEMICOLON: return "<token:semi-colon>";
        case PESEC_TOKEN_COLON: return "<token:colon>";
        case PESEC_TOKEN_QUESTION_MARK: return "<token:question-mark>";
        case PESEC_TOKEN_KEYWORD_IMMUT: return "<token:keyword:immut>";
        case PESEC_TOKEN_KEYWORD_MUT: return "<token:keyword:mut>";
        case PESEC_TOKEN_KEYWORD_FN: return "<token:keyword:fn>";
        case PESEC_TOKEN_KEYWORD_OBJECT: return "<token:keyword:object>";
        case PESEC_TOKEN_KEYWORD_VECTOR: return "<token:keyword:vector>";
    }

    pesec_panic("token::to_cstr: unknown token type %d", type);
}
