#include "include/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/utils/throw.h"
#include "../include/string_value.h"


lexer_t* lexer_new(char* source, const ull_t length)
{
    const auto lexer = (lexer_t*)malloc(sizeof(lexer_t));

    lexer->source = source;
    lexer->line = 1;
    lexer->i = 0;
    lexer->length = length;
    lexer->commenting = false;

    return lexer;
}

void lexer_advance(lexer_t* lexer)
{
    if (!lexer_can_advance(lexer))
    {
        THROW("EOF reached, can't advance at line %llu\n", lexer->line);
    }

    ++lexer->i;
}

bool lexer_can_advance(const lexer_t* lexer)
{
    return lexer->i < lexer->length;
}

char lexer_get_current_char(const lexer_t* lexer)
{
    return lexer->source[lexer->i];
}

void lexer_skip_every_unnecessary_shit(lexer_t* lexer)
{
    while (lexer_can_advance(lexer))
    {
        const char current = lexer_get_current_char(lexer);
        if (current == '\n') ++lexer->line;
        if (lexer->commenting)
        {
            if (current == '#') lexer->commenting = false;

            lexer_advance(lexer);
            continue;
        }

        if (isspace(current))
        {
            lexer_advance(lexer);
            continue;
        }

        if (current == '#')
        {
            lexer_advance(lexer);
            lexer->commenting = true;
            continue;
        }

        break;
    }
}

token_t lexer_next_token(lexer_t* lexer)
{
    lexer_skip_every_unnecessary_shit(lexer);

    if (!lexer_can_advance(lexer)) return (token_t) {
        .line = lexer->line,
        .value = (token_value_t){ .as_string_view = string_view_from("\0") },
        .type = TOKEN_TYPE_EOF
    };

    const char current = lexer_get_current_char(lexer);

    if (isdigit(current)) return lexer_next_number(lexer);
    if (isalpha(current) || current == '_') return lexer_next_identifier(lexer);
    if (current == '"' || current == '\'' || current == '`') return lexer_next_string(lexer, current);
    return lexer_next_operator(lexer);
}

token_t lexer_next_number(lexer_t* lexer)
{
    const unsigned long long begin = lexer->i;
    bool has_dot = false;

    while (lexer_can_advance(lexer) && (isdigit(lexer_get_current_char(lexer)) || lexer_get_current_char(lexer) == '.'))
    {
        if (lexer_get_current_char(lexer) == '.')
        {
            if (has_dot) THROW("Invalid number at line %llu\n", lexer->line);
            has_dot = true;
        }
        lexer_advance(lexer);
    }

    const auto value = (string_view_t) {
        .data = lexer->source + begin,
        .length = lexer->i - begin,
    };

    return (token_t) {
        .line = lexer->line,
        .value.as_number = number_value_from_sv(value),
        .type = TOKEN_TYPE_NUMBER,
    };
}


token_t lexer_next_identifier(lexer_t* lexer)
{
    const ull_t begin = lexer->i;

    while (lexer_can_advance(lexer) && (
            isalnum(lexer_get_current_char(lexer)) ||
            lexer_get_current_char(lexer) == '_'
            )) lexer_advance(lexer);

    const auto value = (string_view_t) {
        .data = lexer->source + begin,
        .length = lexer->i - begin,
    };
    token_type_t type = TOKEN_TYPE_IDENTIFIER;

    if (string_view_equals_cstr(value, "mutab") ||
        string_view_equals_cstr(value, "const") ||
        string_view_equals_cstr(value, "fn") ||
        string_view_equals_cstr(value, "if") ||
        string_view_equals_cstr(value, "else") ||
        string_view_equals_cstr(value, "while") ||
        string_view_equals_cstr(value, "for") ||
        string_view_equals_cstr(value, "in") ||
        string_view_equals_cstr(value, "break") ||
        string_view_equals_cstr(value, "panic") ||
        string_view_equals_cstr(value, "return") ||
        string_view_equals_cstr(value, "struct") ||
        string_view_equals_cstr(value, "import") ||
        string_view_equals_cstr(value, "true") ||
        string_view_equals_cstr(value, "false") ||
        string_view_equals_cstr(value, "null")
        ) type = TOKEN_TYPE_KEYWORD;

    return (token_t) {
        .line = lexer->line,
        .value.as_string_view = value,
        .type = type,
    };
}

token_t lexer_next_string(lexer_t* lexer, const char quote)
{
    lexer_advance(lexer);

    string_value_t* string = string_value_new();

    char current;
    while (lexer_can_advance(lexer) && (current = lexer_get_current_char(lexer)) != quote)
    {
        if (current == '\\')
        {
            lexer_advance(lexer);
            switch (lexer_get_current_char(lexer))
            {
                case '"': string_value_push_back(string, '"'); break;
                case '`': string_value_push_back(string, '`'); break;
                case '\'': string_value_push_back(string, '\''); break;
                case 'n': string_value_push_back(string, '\n'); break;
                case 't': string_value_push_back(string, '\t'); break;
                case 'r': string_value_push_back(string, '\r'); break;
                case 'b': string_value_push_back(string, '\b'); break;
                default: break;
            }
        }
        else
        {
            string_value_push_back(string, current);
        }
        lexer_advance(lexer);
    }

    lexer_advance(lexer);

    return (token_t) {
        .line = lexer->line,
        .value.as_string = string,
        .type = TOKEN_TYPE_STRING, // todo: add format string (`text {}`)
    };
}

token_t lexer_next_operator(lexer_t* lexer)
{
    const char current_char = lexer_get_current_char(lexer);
    switch (current_char)
    {
        case '@': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_AT_SIGN);
        case '.': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_DOT);
        case ',': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_COMMA);
        case ';': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_SEMICOLON);
        case '(': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_LPAREN);
        case ')': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_RPAREN);
        case '{': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_LBRACE);
        case '}': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_RBRACE);
        case '[': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_LBRACKET);
        case ']': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_RBRACKET);

        case '?': lexer_advance(lexer); return LEXER_NEW_TOKEN_FROM_CURRENT_POS(0, 1, TOKEN_TYPE_QUESTION_MARK);
        case '!':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_NOT_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_EXCLAMATION_MARK);
        case '=':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_EQUALS_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_EQUALS);
        case '<':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_LESS_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_LESS);
        case '>':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_GREATER_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_GREATER);
        case '&':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '&')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_AMPERSAND_AMPERSAND);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_AMPERSAND);
        case '|':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '|')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_PIPE_PIPE);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_PIPE);
        case '+':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_PLUS_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_PLUS);
        case '-':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_MINUS_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_MINUS);
        case '*':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '*')
            {
                lexer_advance(lexer);
                if (lexer_get_current_char(lexer) == '=')
                {
                    lexer_advance(lexer);
                    return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 3, TOKEN_TYPE_ASTERISK_ASTERISK_EQUALS);
                }
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_ASTERISK_ASTERISK);
            }
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_ASTERISK_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_ASTERISK);
        case '/':
            lexer_advance(lexer);
            if (lexer_get_current_char(lexer) == '/')
            {
                lexer_advance(lexer);

                if (lexer_get_current_char(lexer) == '=')
                {
                    lexer_advance(lexer);
                    return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 3, TOKEN_TYPE_SLASH_SLASH_EQUALS);
                }
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_SLASH_SLASH);
            }
            if (lexer_get_current_char(lexer) == '=')
            {
                lexer_advance(lexer);
                return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 2, TOKEN_TYPE_SLASH_EQUALS);
            }
            return LEXER_NEW_TOKEN_FROM_CURRENT_POS(-1, 1, TOKEN_TYPE_SLASH);
        default:
            THROW("Unknown character: '%c' at line %llu\n", current_char, lexer->line);
    }
}

void lexer_free(lexer_t* lexer)
{
    free(lexer);
}
