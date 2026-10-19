#include "include/frontend/parser.h"

#include "include/common/node_vector.h"
#include "include/common/panic.h"
#include "include/common/parameter_vector.h"


typedef enum
{
    PESEC_PARSER_BINDING_POWER_NONE,
    PESEC_PARSER_BINDING_POWER_CONDITIONAL,
    PESEC_PARSER_BINDING_POWER_COMPARISON,
    PESEC_PARSER_BINDING_POWER_ADDITIVE,
    PESEC_PARSER_BINDING_POWER_MULTIPLICATIVE,
} pesec_parser_binding_power_t;

static pesec_parser_binding_power_t pesec_parser_infix_binding_power(const pesec_token_type_t type)
{
    switch (type)
    {
        case PESEC_TOKEN_EQUAL_EQUAL:
        case PESEC_TOKEN_LESS_THAN:
        case PESEC_TOKEN_LESS_EQUAL:
        case PESEC_TOKEN_GREATER_THAN:
        case PESEC_TOKEN_GREATER_EQUAL:
            return PESEC_PARSER_BINDING_POWER_COMPARISON;
        case PESEC_TOKEN_QUESTION_MARK:
            return PESEC_PARSER_BINDING_POWER_CONDITIONAL;
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
    pesec_parser_binding_power_t left_binding_power
);

static bool pesec_parser_match(const pesec_parser_t* parser, const pesec_token_type_t type)
{
    return parser->token.type == type;
}

static pesec_token_t pesec_parser_advance(pesec_parser_t* parser)
{
    const pesec_token_t token = parser->token;
    parser->token = pesec_lexer_next(parser->lexer);
    return token;
}

static pesec_token_t pesec_parser_eat(pesec_parser_t* parser, const pesec_token_type_t type)
{
    if (!pesec_parser_match(parser, type))
        pesec_panic("parser::eat: unexpected token (%s but %s given)",
                    pesec_token_type_to_cstr(parser->token.type),
                    pesec_token_type_to_cstr(type)
        );

    return pesec_parser_advance(parser);
}

static pesec_node_t* pesec_parser_parse_postfix(pesec_parser_t* parser, pesec_node_t* node)
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
            default: return node;
        }
    }
}

static pesec_node_t* pesec_parser_parse_block(pesec_parser_t* parser, const bool consider_braces)
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

static pesec_node_t* pesec_parser_parse_primary(pesec_parser_t* parser)
{
    switch (parser->token.type)
    {
        case PESEC_TOKEN_INTEGER:
            return pesec_literal_node_new(
                pesec_value_i64(pesec_parser_eat(parser, PESEC_TOKEN_INTEGER).data.integer)
            );
        case PESEC_TOKEN_IDENTIFIER: {
            const pesec_string_view_t identifier = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
            if (pesec_parser_match(parser, PESEC_TOKEN_EQUAL))
            {
                pesec_parser_eat(parser, PESEC_TOKEN_EQUAL);
                pesec_node_t* node_value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                return pesec_assignment_node_new(identifier, node_value);
            }
            return pesec_variable_node_new(identifier);
        }
        case PESEC_TOKEN_KEYWORD_IMMUT:
        case PESEC_TOKEN_KEYWORD_MUT: {
            const pesec_token_type_t token_type = parser->token.type;
            pesec_parser_advance(parser);
            const pesec_string_view_t name = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
            pesec_parser_eat(parser, PESEC_TOKEN_EQUAL);
            pesec_node_t* value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            return pesec_definition_node_new(
                name,
                value,
                (bool)(token_type == PESEC_TOKEN_KEYWORD_IMMUT)
            );
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
            return pesec_function_node_new(body, parameter_vector);
        }
        case PESEC_TOKEN_KEYWORD_VECTOR: {
            pesec_parser_advance(parser);
            pesec_parser_eat(parser, PESEC_TOKEN_LBRACE);
            pesec_node_vector_t node_vector = pesec_node_vector_new();
            while (!pesec_parser_match(parser, PESEC_TOKEN_RBRACE))
            {
                pesec_node_t* node_item = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                pesec_node_vector_push(&node_vector, node_item);

                if (!pesec_parser_match(parser, PESEC_TOKEN_COMMA)) break;

                pesec_parser_eat(parser, PESEC_TOKEN_COMMA);
            }
            pesec_parser_eat(parser, PESEC_TOKEN_RBRACE);
            return pesec_vector_node_new(node_vector);
        }
        case PESEC_TOKEN_LPAREN: {
            pesec_parser_eat(parser, PESEC_TOKEN_LPAREN);
            pesec_node_t* node = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_parser_eat(parser, PESEC_TOKEN_RPAREN);
            return node;
        }
        case PESEC_TOKEN_LBRACE: {
            return pesec_parser_parse_block(parser, true);
        }
        default:
            pesec_panic("parser::parse_primary: unexpected token %s",
                        pesec_token_type_to_cstr(parser->token.type)
            );
    }
}

static pesec_binary_node_operation_t pesec_parser_binary_operation_from_token(const pesec_token_type_t type)
{
    switch (type)
    {
        case PESEC_TOKEN_PLUS: return PESEC_BINARY_NODE_OPERATION_ADD;
        case PESEC_TOKEN_MINUS: return PESEC_BINARY_NODE_OPERATION_SUB;
        case PESEC_TOKEN_ASTERISK: return PESEC_BINARY_NODE_OPERATION_MUL;
        case PESEC_TOKEN_SLASH: return PESEC_BINARY_NODE_OPERATION_DIV;
        case PESEC_TOKEN_EQUAL_EQUAL: return PESEC_BINARY_NODE_OPERATION_EQUAL;
        case PESEC_TOKEN_LESS_THAN: return PESEC_BINARY_NODE_OPERATION_LESS;
        case PESEC_TOKEN_LESS_EQUAL: return PESEC_BINARY_NODE_OPERATION_LESS_EQUAL;
        case PESEC_TOKEN_GREATER_THAN: return PESEC_BINARY_NODE_OPERATION_GREATER;
        case PESEC_TOKEN_GREATER_EQUAL: return PESEC_BINARY_NODE_OPERATION_GREATER_EQUAL;
        default:
            pesec_panic("parser::binary_operation_from_token: unexpected token %s",
                        pesec_token_type_to_cstr(type));
    }
}

static pesec_node_t* pesec_parser_parse_expression(
    pesec_parser_t* parser,
    const pesec_parser_binding_power_t left_binding_power
)
{
    pesec_node_t* left = pesec_parser_parse_primary(parser);

    for (;;)
    {
        left = pesec_parser_parse_postfix(parser, left);

        const pesec_token_type_t operator_token_type = parser->token.type;
        const pesec_parser_binding_power_t right_binding_power = pesec_parser_infix_binding_power(operator_token_type);

        if (right_binding_power == PESEC_PARSER_BINDING_POWER_NONE ||
            right_binding_power <= left_binding_power)
            break;

        if (operator_token_type == PESEC_TOKEN_QUESTION_MARK)
        {
            pesec_parser_eat(parser, PESEC_TOKEN_QUESTION_MARK);
            pesec_node_t* then_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_parser_eat(parser, PESEC_TOKEN_COLON);
            pesec_node_t* else_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            left = pesec_conditional_node_new(left, then_body, else_body);
            continue;
        }

        const pesec_token_t operator_token = pesec_parser_advance(parser);

        pesec_node_t* right = pesec_parser_parse_expression(parser, right_binding_power);

        const pesec_binary_node_operation_t operation = pesec_parser_binary_operation_from_token(operator_token.type);

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
    return pesec_parser_parse_block(parser, false);
}
