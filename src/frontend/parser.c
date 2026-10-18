#include "include/frontend/parser.h"

#include "include/common/node_vector.h"
#include "include/common/panic.h"
#include "include/common/parameter_vector.h"


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
        pesec_panic("parser::eat: unexpected token (%s but %s given)",
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


static pesec_node_t* pesec_parser_parse_postfix(
    pesec_parser_t* parser,
    pesec_node_t* node
)
{
    for (;;)
    {
        switch (parser->token.type)
        {
            case PESEC_TOKEN_LPAREN: {
                pesec_parser_eat(parser, PESEC_TOKEN_LPAREN);
                pesec_node_vector_t node_vector = pesec_node_vector_new();
                while (!pesec_parser_match(parser, PESEC_TOKEN_RPAREN))
                {
                    pesec_node_t* node_item = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                    pesec_node_vector_push(&node_vector, node_item);

                    if (!pesec_parser_match(parser, PESEC_TOKEN_COMMA)) break;

                    pesec_parser_eat(parser, PESEC_TOKEN_COMMA);
                }

                pesec_parser_eat(parser, PESEC_TOKEN_RPAREN);
                node = pesec_call_node_new(node, node_vector);
                break;
            }
            case PESEC_TOKEN_QUESTION_MARK: {
                pesec_parser_eat(parser, PESEC_TOKEN_QUESTION_MARK);
                pesec_node_t* then_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                pesec_parser_eat(parser, PESEC_TOKEN_COLON);
                pesec_node_t* else_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                node = pesec_conditional_node_new(node, then_body, else_body);
                break;
            }
            default: return node;
        }
    }

    return node;
}

static pesec_node_t* pesec_parser_parse_block(pesec_parser_t* parser, bool consider_braces)
{
    pesec_node_vector_t node_vector = pesec_node_vector_new();

    if (consider_braces)
        pesec_parser_eat(parser, PESEC_TOKEN_LBRACE);

    while (parser->token.type != PESEC_TOKEN_EOF &&
           parser->token.type != PESEC_TOKEN_RBRACE)
    {
        pesec_node_vector_push(
            &node_vector,
            pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE)
        );
        pesec_parser_eat(parser, PESEC_TOKEN_SEMICOLON);
    }

    if (consider_braces)
        pesec_parser_eat(parser, PESEC_TOKEN_RBRACE);

    return pesec_sequence_node_new(node_vector);
}

static pesec_node_t* pesec_parser_parse_expression(
    pesec_parser_t* parser,
    const pesec_parser_binding_power_t left_binding_power
)
{
    pesec_node_t* left = nullptr;

    switch (parser->token.type)
    {
        case PESEC_TOKEN_INTEGER: {
            left = pesec_literal_node_new(
                pesec_value_i64(pesec_parser_eat(parser, PESEC_TOKEN_INTEGER).data.integer)
            );
            break;
        }
        case PESEC_TOKEN_IDENTIFIER: {
            const pesec_string_view_t identifier = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
            if (pesec_parser_match(parser, PESEC_TOKEN_EQUAL))
            {
                pesec_parser_eat(parser, PESEC_TOKEN_EQUAL);
                pesec_node_t* node_value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                left = pesec_assignment_node_new(identifier, node_value);
                break;
            }
            left = pesec_variable_node_new(identifier);
            break;
        }
        case PESEC_TOKEN_KEYWORD_IMMUT:
        case PESEC_TOKEN_KEYWORD_MUT: {
            const pesec_token_type_t token_type = parser->token.type;
            pesec_parser_eat(parser, token_type);
            const pesec_string_view_t name = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
            pesec_parser_eat(parser, PESEC_TOKEN_EQUAL);
            pesec_node_t* value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            left = pesec_definition_node_new(
                name,
                value,
                (bool)(token_type == PESEC_TOKEN_KEYWORD_IMMUT)
            );
            break;
        }
        case PESEC_TOKEN_KEYWORD_FN: {
            pesec_parser_eat(parser, PESEC_TOKEN_KEYWORD_FN);
            pesec_parser_eat(parser, PESEC_TOKEN_LPAREN);
            pesec_parameter_vector_t parameter_vector = pesec_parameter_vector_new();
            while (!pesec_parser_match(parser, PESEC_TOKEN_RPAREN))
            {
                const pesec_token_t parameter_token = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER);
                const auto parameter = (pesec_parameter_t){
                    .name = parameter_token.data.string_view
                };
                pesec_parameter_vector_push(&parameter_vector, parameter);

                if (!pesec_parser_match(parser, PESEC_TOKEN_COMMA)) break;

                pesec_parser_eat(parser, PESEC_TOKEN_COMMA);
            }
            pesec_parser_eat(parser, PESEC_TOKEN_RPAREN);
            pesec_node_t* body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            left = pesec_function_node_new(body, parameter_vector);
            break;
        }
        case PESEC_TOKEN_LPAREN: {
            pesec_parser_eat(parser, PESEC_TOKEN_LPAREN);
            left = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_ADDITIVE);
            pesec_parser_eat(parser, PESEC_TOKEN_RPAREN);
            break;
        }
        case PESEC_TOKEN_LBRACE: {
            left = pesec_parser_parse_block(parser, true);
            break;
        }
        default:
            pesec_panic("parser::parse_primary: unexpected token %s",
                        pesec_token_type_to_cstr(parser->token.type)
            );
    }

    for (;;)
    {
        const pesec_parser_binding_power_t right_binding_power = pesec_parser_infix_binding_power(parser->token.type);

        if (right_binding_power == PESEC_PARSER_BINDING_POWER_NONE ||
            right_binding_power < left_binding_power)
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

    return pesec_parser_parse_postfix(parser, left);
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
    return pesec_parser_parse_block(parser, false);
}
