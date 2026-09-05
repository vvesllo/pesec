#include "include/token.h"

#include <stdio.h>

void token_print(FILE* stream, const token_t token)
{
    switch (token.type)
    {
        case TOKEN_TYPE_NUMBER:
            fprintf(stream, "number: %Lf", number_value_to_long_double(token.value.as_number)); break;
        case TOKEN_TYPE_IDENTIFIER:
            fprintf(stream, "identifier: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_KEYWORD:
            fprintf(stream, "keyword: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_STRING:
            fprintf(stream, "string: %s", token.value.as_string->data); break;
        case TOKEN_TYPE_EQUALS:
            fprintf(stream, "assign: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_EQUALS_EQUALS:
        case TOKEN_TYPE_LESS:
        case TOKEN_TYPE_LESS_EQUALS:
        case TOKEN_TYPE_GREATER:
        case TOKEN_TYPE_GREATER_EQUALS:
        case TOKEN_TYPE_NOT_EQUALS:
            fprintf(stream, "comparison: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_PLUS:
        case TOKEN_TYPE_MINUS:
        case TOKEN_TYPE_SLASH:
        case TOKEN_TYPE_SLASH_SLASH:
        case TOKEN_TYPE_ASTERISK:
        case TOKEN_TYPE_ASTERISK_ASTERISK:
            fprintf(stream, "operator: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_LPAREN:
        case TOKEN_TYPE_RPAREN:
            fprintf(stream, "paren: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_LBRACE:
        case TOKEN_TYPE_RBRACE:
            fprintf(stream, "brace: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_LBRACKET:
        case TOKEN_TYPE_RBRACKET:
            fprintf(stream, "bracket: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_COMMA:
            fprintf(stream, "comma: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_DOT:
            fprintf(stream, "dot: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_AT_SIGN:
            fprintf(stream, "at: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_SEMICOLON:
            fprintf(stream, "semicolon: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_QUESTION_MARK:
        case TOKEN_TYPE_EXCLAMATION_MARK:
            fprintf(stream, "mark: %.*s", (unsigned int)token.value.as_string_view.length, token.value.as_string_view.data); break;
        case TOKEN_TYPE_EOF:
            fprintf(stream, "end of file"); break;
    }
}
