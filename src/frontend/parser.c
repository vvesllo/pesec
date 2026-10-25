#include "include/frontend/parser.h"

#include "include/common/node_vector.h"
#include "include/common/panic.h"
#include "include/common/parameter_vector.h"
#include "include/runtime/number_value.h"
#include "include/runtime/string_value.h"


typedef enum
{
    PESEC_PARSER_BINDING_POWER_NONE,
    PESEC_PARSER_BINDING_POWER_RANGE,
    PESEC_PARSER_BINDING_POWER_COMPARISON,
    PESEC_PARSER_BINDING_POWER_ADDITIVE,
    PESEC_PARSER_BINDING_POWER_MULTIPLICATIVE,
    PESEC_PARSER_BINDING_POWER_UNARY,
} pesec_parser_binding_power_t;

static pesec_parser_binding_power_t pesec_parser_infix_binding_power(const pesec_token_type_t type)
{
    switch (type)
    {
        case PESEC_TOKEN_DOT_DOT:
        case PESEC_TOKEN_DOT_DOT_DOT:
            return PESEC_PARSER_BINDING_POWER_RANGE;
        case PESEC_TOKEN_EQUAL_EQUAL:
        case PESEC_TOKEN_LESS_THAN:
        case PESEC_TOKEN_LESS_EQUAL:
        case PESEC_TOKEN_GREATER_THAN:
        case PESEC_TOKEN_GREATER_EQUAL:
            return PESEC_PARSER_BINDING_POWER_COMPARISON;
        case PESEC_TOKEN_PLUS:
        case PESEC_TOKEN_MINUS:
            return PESEC_PARSER_BINDING_POWER_ADDITIVE;
        case PESEC_TOKEN_ASTERISK:
        case PESEC_TOKEN_SLASH:
        case PESEC_TOKEN_PERCENT:
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
        pesec_panic("parser::eat(%zu:%zu): unexpected token (%s but %s given)",
                    parser->token.line + 1,
                    parser->token.column,
                    pesec_token_type_to_cstr(parser->token.type),
                    pesec_token_type_to_cstr(type)
        );

    return pesec_parser_advance(parser);
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
            case PESEC_TOKEN_LBRACKET: {
                pesec_parser_eat(parser, PESEC_TOKEN_LBRACKET);
                pesec_node_t* index_node = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                pesec_parser_eat(parser, PESEC_TOKEN_RBRACKET);
                node = pesec_index_access_node_new(node, index_node);
                break;
            }
            case PESEC_TOKEN_DOT: {
                pesec_parser_advance(parser);
                const pesec_string_view_t field = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
                node = pesec_field_access_node_new(node, field);
                break;
            }
            case PESEC_TOKEN_EQUAL: {
                pesec_parser_advance(parser);
                pesec_node_t* node_value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                node = pesec_assignment_node_new(node, node_value);
                break;
            }
            default: return node;
        }
    }
}

static pesec_node_t* pesec_parser_parse_primary(pesec_parser_t* parser)
{
    switch (parser->token.type)
    {
        case PESEC_TOKEN_MINUS: {
            pesec_parser_advance(parser);
            pesec_node_t* operand = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_UNARY);
            return pesec_unary_node_new(PESEC_UNARY_NODE_OPERATION_NEGATIVE, operand);
        }
        case PESEC_TOKEN_NUMBER: {
            return pesec_literal_node_new(
                pesec_value_as_number(pesec_number_value_new(pesec_parser_advance(parser).data.number))
            );
        }
        case PESEC_TOKEN_STRING: {
            return pesec_literal_node_new(
                pesec_value_as_string(pesec_string_value_from(pesec_parser_advance(parser).data.string_view))
            );
        }
        case PESEC_TOKEN_IDENTIFIER: {
            const pesec_string_view_t identifier = pesec_parser_advance(parser).data.string_view;
            return pesec_variable_node_new(identifier);
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
            pesec_parser_advance(parser);
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
            pesec_node_t* body = pesec_parser_parse_primary(parser);
            return pesec_function_node_new(body, parameter_vector);
        }
        case PESEC_TOKEN_KEYWORD_IF: {
            pesec_parser_advance(parser);
            pesec_node_t* condition = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_node_t* then_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_node_t* else_body = nullptr;
            if (pesec_parser_match(parser, PESEC_TOKEN_KEYWORD_ELSE))
            {
                pesec_parser_advance(parser);
                else_body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            }
            return pesec_conditional_node_new(condition, then_body, else_body);
        }
        case PESEC_TOKEN_KEYWORD_CONTINUE:
        case PESEC_TOKEN_KEYWORD_BREAK:
        case PESEC_TOKEN_KEYWORD_RETURN: {
            pesec_value_control_flow_t control_flow = PESEC_VALUE_CONTROL_FLOW_NONE;
            switch (parser->token.type)
            {
                case PESEC_TOKEN_KEYWORD_CONTINUE: control_flow = PESEC_VALUE_CONTROL_FLOW_CONTINUE;
                    break;
                case PESEC_TOKEN_KEYWORD_BREAK: control_flow = PESEC_VALUE_CONTROL_FLOW_BREAK;
                    break;
                case PESEC_TOKEN_KEYWORD_RETURN: control_flow = PESEC_VALUE_CONTROL_FLOW_RETURN;
                    break;
                default: control_flow = PESEC_VALUE_CONTROL_FLOW_NONE;
                    break;
            }
            pesec_parser_advance(parser);
            pesec_node_t* value = nullptr;
            if (!pesec_parser_match(parser, PESEC_TOKEN_SEMICOLON))
            {
                value = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            }
            return pesec_control_flow_node_new(value, control_flow);
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
        case PESEC_TOKEN_KEYWORD_OBJECT: {
            pesec_parser_advance(parser);
            pesec_parser_eat(parser, PESEC_TOKEN_LBRACE);
            pesec_node_vector_t node_vector = pesec_node_vector_new();
            while (!pesec_parser_match(parser, PESEC_TOKEN_RBRACE))
            {
                pesec_node_t* node_item = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
                pesec_node_vector_push(&node_vector, node_item);

                if (!pesec_parser_match(parser, PESEC_TOKEN_SEMICOLON)) break;

                pesec_parser_eat(parser, PESEC_TOKEN_SEMICOLON);
            }
            pesec_parser_eat(parser, PESEC_TOKEN_RBRACE);
            return pesec_object_node_new(node_vector);
        }
        case PESEC_TOKEN_KEYWORD_FOR: {
            pesec_parser_advance(parser);
            const pesec_string_view_t iterator = pesec_parser_eat(parser, PESEC_TOKEN_IDENTIFIER).data.string_view;
            pesec_parser_eat(parser, PESEC_TOKEN_KEYWORD_IN);
            pesec_node_t* iterable = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_node_t* body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            return pesec_for_node_new(iterator, iterable, body);
        }
        case PESEC_TOKEN_KEYWORD_WHILE: {
            pesec_parser_advance(parser);
            pesec_node_t* condition = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            pesec_node_t* body = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            return pesec_while_node_new(condition, body);
        }
        default:
            pesec_panic("parser::parse_primary(%zu:%zu): unexpected token %s",
                        parser->token.line + 1,
                        parser->token.column,
                        pesec_token_type_to_cstr(parser->token.type)
            );
    }
}

static pesec_binary_node_operation_t pesec_parser_binary_operation_from_token(const pesec_token_t token)
{
    switch (token.type)
    {
        case PESEC_TOKEN_PLUS: return PESEC_BINARY_NODE_OPERATION_ADD;
        case PESEC_TOKEN_MINUS: return PESEC_BINARY_NODE_OPERATION_SUB;
        case PESEC_TOKEN_ASTERISK: return PESEC_BINARY_NODE_OPERATION_MUL;
        case PESEC_TOKEN_SLASH: return PESEC_BINARY_NODE_OPERATION_DIV;
        case PESEC_TOKEN_PERCENT: return PESEC_BINARY_NODE_OPERATION_MOD;
        case PESEC_TOKEN_EQUAL_EQUAL: return PESEC_BINARY_NODE_OPERATION_EQUAL;
        case PESEC_TOKEN_LESS_THAN: return PESEC_BINARY_NODE_OPERATION_LESS;
        case PESEC_TOKEN_LESS_EQUAL: return PESEC_BINARY_NODE_OPERATION_LESS_EQUAL;
        case PESEC_TOKEN_GREATER_THAN: return PESEC_BINARY_NODE_OPERATION_GREATER;
        case PESEC_TOKEN_GREATER_EQUAL: return PESEC_BINARY_NODE_OPERATION_GREATER_EQUAL;
        default:
            pesec_panic("parser::binary_operation_from_token(%zu:%zu): unexpected token %s",
                        token.line + 1,
                        token.column,
                        pesec_token_type_to_cstr(token.type));
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

        if (operator_token_type == PESEC_TOKEN_DOT_DOT || operator_token_type == PESEC_TOKEN_DOT_DOT_DOT)
        {
            const bool contains = parser->token.type == PESEC_TOKEN_DOT_DOT_DOT;
            pesec_parser_advance(parser);
            pesec_node_t* end_node = pesec_parser_parse_expression(parser, PESEC_PARSER_BINDING_POWER_NONE);
            left = pesec_range_node_new(left, end_node, contains);
            continue;
        }


        const pesec_token_t operator_token = pesec_parser_advance(parser);

        pesec_node_t* right = pesec_parser_parse_expression(parser, right_binding_power);

        const pesec_binary_node_operation_t operation = pesec_parser_binary_operation_from_token(operator_token);

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
