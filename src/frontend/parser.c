#include "include/frontend/parser.h"

#include "include/common/panic.h"


typedef enum
{
    PESEC_PARSER_BINDING_POWER_NONE,
    PESEC_PARSER_BINDING_POWER_ADDITIVE,
    PESEC_PARSER_BINDING_POWER_MULTIPLICATIVE,
} pesec_parser_binding_power_t;

static pesec_node_t* pesec_parser_parse_expression(
    pesec_parser_t* parser,
    pesec_parser_binding_power_t left_binding_power
);

static bool pesec_parser_match(const pesec_parser_t* parser, const pesec_token_type_t type)
{
    return parser->token.type == type;
}

static pesec_token_t pesec_parser_eat(pesec_parser_t* parser, const pesec_token_type_t type)
{
    if (!pesec_parser_match(parser, type))
        pesec_panic("parser::eat: unexpected token(%s but %s given)",
                    pesec_token_type_to_cstr(parser->token.type),
                    pesec_token_type_to_cstr(type)
        );

    const pesec_token_t token = parser->token;

    parser->token = pesec_lexer_next(parser->lexer);

    return token;
}

static pesec_parser_binding_power_t pesec_parser_infix_binding_power(const pesec_token_type_t type)
{
    switch (type)
    {
        case PESEC_TOKEN_PLUS:
        case PESEC_TOKEN_MINUS:
            return PESEC_PARSER_BINDING_POWER_ADDITIVE;
        case PESEC_TOKEN_ASTERISK:
        case PESEC_TOKEN_SLASH:
            return PESEC_PARSER_BINDING_POWER_MULTIPLICATIVE;
        default:
            return PESEC_PARSER_BINDING_POWER_NONE;
    }
}

static pesec_node_t* pesec_parser_parse_expression(
    pesec_parser_t* parser,
    const pesec_parser_binding_power_t left_binding_power
)
{
    pesec_node_t* left;

    switch (parser->token.type)
    {
        case PESEC_TOKEN_INTEGER:
            left = pesec_literal_node_new(pesec_parser_eat(parser, PESEC_TOKEN_INTEGER).data.integer);
            break;
        case PESEC_TOKEN_LPAREN:
            pesec_parser_eat(parser, PESEC_TOKEN_LPAREN);
            left = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_ADDITIVE);
            pesec_parser_eat(parser, PESEC_TOKEN_RPAREN);
            break;
        default:
            pesec_panic("parser::parse_primary: unexpected token %s",
                        pesec_token_type_to_cstr(parser->token.type)
            );
    }

    for (;;)
    {
        const pesec_parser_binding_power_t right_binding_power = pesec_parser_infix_binding_power(parser->token.type);

        if (right_binding_power == PESEC_PARSER_BINDING_POWER_NONE || right_binding_power < left_binding_power)
            break;

        const pesec_token_t operator_token = pesec_parser_eat(parser, parser->token.type);
        pesec_node_t* right = pesec_parser_parse_expression(parser, right_binding_power);

        pesec_binary_node_operation_t operation = PESEC_BINARY_NODE_OPERATION_ADD;

        switch (operator_token.type)
        {
            case PESEC_TOKEN_PLUS: operation = PESEC_BINARY_NODE_OPERATION_ADD;
                break;
            case PESEC_TOKEN_MINUS: operation = PESEC_BINARY_NODE_OPERATION_SUB;
                break;
            case PESEC_TOKEN_ASTERISK: operation = PESEC_BINARY_NODE_OPERATION_MUL;
                break;
            case PESEC_TOKEN_SLASH: operation = PESEC_BINARY_NODE_OPERATION_DIV;
                break;
            default: break;
        }

        left = pesec_binary_node_new(operation, left, right);
    }

    return left;
}

pesec_parser_t pesec_parser_new(pesec_lexer_t* lexer)
{
    pesec_parser_t parser;
    parser.lexer = lexer;
    parser.token = pesec_lexer_next(parser.lexer);
    return parser;
}

pesec_node_t* pesec_parser_parse(pesec_parser_t* parser)
{
    return pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_ADDITIVE);
}
