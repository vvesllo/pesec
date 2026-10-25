#include "include/frontend/lexer.h"

#include <ctype.h>

#include "include/common/panic.h"


static void pesec_lexer_advance(pesec_lexer_t* lexer)
{
    lexer->i += 1;
    lexer->column += 1;
}

static bool pesec_lexer_can_advance(const pesec_lexer_t* lexer)
{
    return lexer->i < lexer->source.size;
}

static char pesec_lexer_peek(const pesec_lexer_t* lexer, const size_t offset)
{
    if (lexer->i + offset >= lexer->source.size)
        return '\0';

    return lexer->source.data[lexer->i + offset];
}

static pesec_token_t pesec_lexer_next_numeric(pesec_lexer_t* lexer)
{
    double integer_part = 0;
    double fraction_part = 0;
    double divisor = 10;
    bool has_dot = false;

    while (pesec_lexer_can_advance(lexer) &&
           (isdigit(pesec_lexer_peek(lexer, 0)) || pesec_lexer_peek(lexer, 0) == '.'))
    {
        const char curr_char = pesec_lexer_peek(lexer, 0);
        const char next_char = pesec_lexer_peek(lexer, 1);

        if (curr_char == '.' && (next_char == '.' || !isdigit(next_char)))
            break;

        if (curr_char == '.')
        {
            has_dot = true;
            pesec_lexer_advance(lexer);
            continue;
        }

        const unsigned char digit = (char)(curr_char - '0');
        if (!has_dot)
            integer_part = integer_part * 10 + digit;
        else
        {
            fraction_part += digit / divisor;
            divisor *= 10;
        }

        pesec_lexer_advance(lexer);
    }

    const double value = integer_part + fraction_part;

    return pesec_token_new(
        PESEC_TOKEN_NUMBER,
        (pesec_token_data_t){
            .number = value
        },
        lexer->line,
        lexer->column
    );
}

static pesec_token_t pesec_lexer_next_identifier(pesec_lexer_t* lexer)
{
    const size_t begin = lexer->i;
    while (pesec_lexer_can_advance(lexer) && (isalnum(pesec_lexer_peek(lexer, 0)) || pesec_lexer_peek(lexer, 0) == '_'))
    {
        pesec_lexer_advance(lexer);
    }

    const pesec_string_view_t value = pesec_string_view_new(
        lexer->source.data + begin,
        lexer->i - begin
    );

    pesec_token_type_t type = PESEC_TOKEN_IDENTIFIER;

    if (pesec_string_view_equals_cstr(value, "immut")) type = PESEC_TOKEN_KEYWORD_IMMUT;
    else if (pesec_string_view_equals_cstr(value, "mut")) type = PESEC_TOKEN_KEYWORD_MUT;
    else if (pesec_string_view_equals_cstr(value, "fn")) type = PESEC_TOKEN_KEYWORD_FN;
    else if (pesec_string_view_equals_cstr(value, "if")) type = PESEC_TOKEN_KEYWORD_IF;
    else if (pesec_string_view_equals_cstr(value, "else")) type = PESEC_TOKEN_KEYWORD_ELSE;
    else if (pesec_string_view_equals_cstr(value, "object")) type = PESEC_TOKEN_KEYWORD_OBJECT;
    else if (pesec_string_view_equals_cstr(value, "vector")) type = PESEC_TOKEN_KEYWORD_VECTOR;
    else if (pesec_string_view_equals_cstr(value, "while")) type = PESEC_TOKEN_KEYWORD_WHILE;
    else if (pesec_string_view_equals_cstr(value, "for")) type = PESEC_TOKEN_KEYWORD_FOR;
    else if (pesec_string_view_equals_cstr(value, "in")) type = PESEC_TOKEN_KEYWORD_IN;
    else if (pesec_string_view_equals_cstr(value, "continue")) type = PESEC_TOKEN_KEYWORD_CONTINUE;
    else if (pesec_string_view_equals_cstr(value, "break")) type = PESEC_TOKEN_KEYWORD_BREAK;
    else if (pesec_string_view_equals_cstr(value, "return")) type = PESEC_TOKEN_KEYWORD_RETURN;

    return pesec_token_new(
        type,
        (pesec_token_data_t){
            .string_view = value
        },
        lexer->line,
        lexer->column
    );
}
static pesec_token_t pesec_lexer_next_string(pesec_lexer_t* lexer, const char quotation_mark)
{
    pesec_lexer_advance(lexer);
    const size_t begin = lexer->i;
    while (pesec_lexer_can_advance(lexer) && pesec_lexer_peek(lexer, 0) != quotation_mark)
    {
        pesec_lexer_advance(lexer);
    }

    const pesec_string_view_t value = pesec_string_view_new(
        lexer->source.data + begin,
        lexer->i - begin
    );

    pesec_lexer_advance(lexer);

    return pesec_token_new(
        PESEC_TOKEN_STRING,
        (pesec_token_data_t){
            .string_view = value
        },
        lexer->line,
        lexer->column
    );
}

static pesec_token_t pesec_lexer_next_operator(pesec_lexer_t* lexer)
{
    switch (pesec_lexer_peek(lexer, 0))
    {
        case '+': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_PLUS, lexer->line, lexer->column);
        case '-': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_MINUS, lexer->line, lexer->column);
        case '*': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_ASTERISK, lexer->line, lexer->column);
        case '/': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_SLASH, lexer->line, lexer->column);
        case '%': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_PERCENT, lexer->line, lexer->column);
        case '=': {
            pesec_lexer_advance(lexer);
            if (pesec_lexer_peek(lexer, 0) == '=')
            {
                pesec_lexer_advance(lexer);
                return pesec_token_blank(PESEC_TOKEN_EQUAL_EQUAL, lexer->line, lexer->column);
            }
            return pesec_token_blank(PESEC_TOKEN_EQUAL, lexer->line, lexer->column);
        }
        case '<': {
            pesec_lexer_advance(lexer);
            if (pesec_lexer_peek(lexer, 0) == '=')
            {
                pesec_lexer_advance(lexer);
                return pesec_token_blank(PESEC_TOKEN_LESS_EQUAL, lexer->line, lexer->column);
            }
            return pesec_token_blank(PESEC_TOKEN_LESS_THAN, lexer->line, lexer->column);
        }
        case '>': {
            pesec_lexer_advance(lexer);
            if (pesec_lexer_peek(lexer, 0) == '=')
            {
                pesec_lexer_advance(lexer);
                return pesec_token_blank(PESEC_TOKEN_GREATER_EQUAL, lexer->line, lexer->column);
            }
            return pesec_token_blank(PESEC_TOKEN_GREATER_THAN, lexer->line, lexer->column);
        }
        case ',': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_COMMA, lexer->line, lexer->column);
        case '.': {
            pesec_lexer_advance(lexer);
            if (pesec_lexer_peek(lexer, 0) == '.')
            {
                pesec_lexer_advance(lexer);
                if (pesec_lexer_peek(lexer, 0) == '.')
                {
                    pesec_lexer_advance(lexer);
                    return pesec_token_blank(PESEC_TOKEN_DOT_DOT_DOT, lexer->line, lexer->column);
                }
                return pesec_token_blank(PESEC_TOKEN_DOT_DOT, lexer->line, lexer->column);
            }
            return pesec_token_blank(PESEC_TOKEN_DOT, lexer->line, lexer->column);
        }
        case ';': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_SEMICOLON, lexer->line, lexer->column);
        case ':': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_COLON, lexer->line, lexer->column);
        case '?': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_QUESTION_MARK, lexer->line, lexer->column);
        case '(': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_LPAREN, lexer->line, lexer->column);
        case ')': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_RPAREN, lexer->line, lexer->column);
        case '{': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_LBRACE, lexer->line, lexer->column);
        case '}': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_RBRACE, lexer->line, lexer->column);
        case '[': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_LBRACKET, lexer->line, lexer->column);
        case ']': pesec_lexer_advance(lexer);
            return pesec_token_blank(PESEC_TOKEN_RBRACKET, lexer->line, lexer->column);
        default: pesec_panic("lexer::next_operator: unknown character `%c`", pesec_lexer_peek(lexer, 0));
    }
}

pesec_lexer_t pesec_lexer_new(const pesec_string_view_t source)
{
    return (pesec_lexer_t){
        .source = source,
        .i = 0,
        .line = 0,
        .column = 0,
    };
}

pesec_token_t pesec_lexer_next(pesec_lexer_t* lexer)
{
    while (pesec_lexer_can_advance(lexer) && isspace(pesec_lexer_peek(lexer, 0)))
    {
        if (pesec_lexer_peek(lexer, 0) == '\n')
        {
            lexer->column = 0;
            lexer->line += 1;
        }
        pesec_lexer_advance(lexer);
    }

    if (!pesec_lexer_can_advance(lexer))
        return pesec_token_blank(PESEC_TOKEN_EOF, lexer->line, lexer->column);

    const char current = pesec_lexer_peek(lexer, 0);
    if (isdigit(current)) return pesec_lexer_next_numeric(lexer);
    if (isalpha(current)) return pesec_lexer_next_identifier(lexer);
    if (current == '"' || current == '\'' || current == '`') return pesec_lexer_next_string(lexer, current);
    return pesec_lexer_next_operator(lexer);
}
