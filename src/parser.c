#include "include/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/utils/throw.h"


parser_t *parser_new(lexer_t *lexer)
{
    const auto parser = (parser_t *) malloc(sizeof(parser_t));

    parser->lexer = lexer;
    parser->current_token = lexer_next_token(parser->lexer);

    return parser;
}

bool parser_match(const parser_t *parser, const token_type_t type)
{
    return parser->current_token.type == type;
}

token_t parser_eat(parser_t *parser, const token_type_t type)
{
    if (!parser_match(parser, type))
    {
        fprintf(stderr, "Unexpected token \"");
        token_print(stderr, parser->current_token);
        fprintf(stderr, "\" at line %llu\n", parser->current_token.line);
        exit(EXIT_FAILURE);
    }
    const token_t prev_token = parser->current_token;
    parser->current_token = lexer_next_token(parser->lexer);

    return prev_token;
}

ast_node_t* parser_check_and_parse_function_call(parser_t *parser, ast_node_t* callee)
{
    if (parser_match(parser, TOKEN_TYPE_LPAREN))
        return parser_parse_function_call(parser, callee);

    return callee;
}

ast_node_t* parser_check_and_parse_variable_field_access(parser_t *parser, ast_node_t* object)
{
    if (parser_match(parser, TOKEN_TYPE_DOT))
        return parser_parse_variable_field_access(parser, object);

    return object;
}

ast_node_t* parser_check_and_parse_variable_assignment(parser_t *parser, ast_node_t* target)
{
    if (parser_match(parser, TOKEN_TYPE_EQUALS))
        return parser_parse_variable_assignment(parser, target);

    return target;
}

ast_node_t* parser_check_and_parse_array_access(parser_t *parser, ast_node_t* array)
{
    if (parser_match(parser, TOKEN_TYPE_LBRACKET))
        return parser_parse_array_access(parser, array);

    return array;
}

ast_node_t* parser_check_and_do_everything(parser_t *parser, ast_node_t* node)
{
    node = parser_check_and_parse_function_call(parser, node);
    node = parser_check_and_parse_variable_field_access(parser, node);
    node = parser_check_and_parse_variable_assignment(parser, node);
    node = parser_check_and_parse_array_access(parser, node);

    return node;
}

ast_node_t *parser_parse(parser_t *parser)
{
    return parser_parse_statement_sequence(parser, false);
}

ast_node_t *parser_parse_statement_sequence(parser_t *parser, const bool between_braces)
{
    ast_node_t *statement_sequence = statement_sequence_node_new(between_braces);

    if (between_braces) parser_eat(parser, TOKEN_TYPE_LBRACE);

    while (!parser_match(parser, TOKEN_TYPE_EOF) && (!between_braces || !parser_match(parser, TOKEN_TYPE_RBRACE)))
    {
        statement_sequence_node_push(statement_sequence->node.statement_sequence, parser_parse_statement(parser));
        if (parser_match(parser, TOKEN_TYPE_SEMICOLON)) parser_eat(parser, TOKEN_TYPE_SEMICOLON);
    }

    if (between_braces) parser_eat(parser, TOKEN_TYPE_RBRACE);

    return statement_sequence;
}

ast_node_t *parser_parse_identifier(parser_t *parser)
{
    const string_view_t name = parser_eat(parser, TOKEN_TYPE_IDENTIFIER).value.as_string_view;

    ast_node_t* variable_node = parser_parse_variable(parser, name);

    if (parser_match(parser, TOKEN_TYPE_LPAREN))    return parser_parse_function_call(parser, variable_node);
    if (parser_match(parser, TOKEN_TYPE_EQUALS))    return parser_parse_variable_assignment(parser, variable_node);
    if (parser_match(parser, TOKEN_TYPE_DOT))       return parser_parse_variable_field_access(parser, variable_node);
    if (parser_match(parser, TOKEN_TYPE_LBRACKET))  return parser_parse_array_access(parser, variable_node);

    return variable_node;
}

ast_node_t *parser_parse_keyword(parser_t *parser)
{
    const string_view_t name = parser_eat(parser, TOKEN_TYPE_KEYWORD).value.as_string_view;

    if (string_view_equals_cstr(name, "true")) return literal_node_new(value_new_boolean(true));
    if (string_view_equals_cstr(name, "false")) return literal_node_new(value_new_boolean(false));

    if (string_view_equals_cstr(name, "mutab")) return parser_parse_variable_definition(parser, false);
    if (string_view_equals_cstr(name, "const")) return parser_parse_variable_definition(parser, true);

    if (string_view_equals_cstr(name, "fn")) return parser_parse_function_definition(parser);
    if (string_view_equals_cstr(name, "struct")) return parser_parse_structure_definition(parser);

    if (string_view_equals_cstr(name, "if")) return parser_parse_if(parser);
    if (string_view_equals_cstr(name, "while")) return parser_parse_while(parser);
    if (string_view_equals_cstr(name, "for")) return parser_parse_for(parser);

    if (string_view_equals_cstr(name, "import")) return parser_parse_import(parser);

    if (string_view_equals_cstr(name, "break")) return parser_parse_break(parser);
    if (string_view_equals_cstr(name, "throw")) return parser_parse_throw(parser);

    THROW("Unknown keyword \"%.*s\" at line %llu", (unsigned int)name.length, name.data, parser->current_token.line);
}

ast_node_t *parser_parse_variable(parser_t *parser, const string_view_t name)
{
    return variable_node_new(name);
}

ast_node_t *parser_parse_variable_definition(parser_t *parser, const bool constant)
{
    const string_view_t name = parser_eat(parser, TOKEN_TYPE_IDENTIFIER).value.as_string_view;

    ast_node_t *value = literal_node_new(value_new_number(NUM_VAL_0));

    if (parser_match(parser, TOKEN_TYPE_EQUALS))
    {
        parser_eat(parser, TOKEN_TYPE_EQUALS);
        value = parser_parse_statement(parser);
    }

    return variable_definition_node_new(name, value, constant);
}

ast_node_t *parser_parse_variable_assignment(parser_t *parser, ast_node_t* target)
{
    parser_eat(parser, TOKEN_TYPE_EQUALS);
    ast_node_t *value = parser_parse_statement(parser);

    return variable_assignment_node_new(target, value);
}

ast_node_t *parser_parse_function_call(parser_t *parser, ast_node_t* callee)
{
    parser_eat(parser, TOKEN_TYPE_LPAREN);

    function_call_argument_node_t* arguments_head = nullptr;
    ull_t count = 0;

    if (!parser_match(parser, TOKEN_TYPE_RPAREN))
    {
        function_call_argument_node_t* arguments_tail = nullptr;
        do
        {
            ast_node_t* value_expr = parser_parse_statement(parser);

            const auto new_arg = (function_call_argument_node_t*)malloc(sizeof(function_call_argument_node_t));
            new_arg->name = (string_view_t){0};
            new_arg->value_expr = value_expr;
            new_arg->next = nullptr;

            if (!arguments_head)
                arguments_head = arguments_tail = new_arg;
            else
            {
                arguments_tail->next = new_arg;
                arguments_tail = new_arg;
            }

            count++;

            if (!parser_match(parser, TOKEN_TYPE_COMMA)) break;
            parser_eat(parser, TOKEN_TYPE_COMMA);
        } while (true);
    }

    parser_eat(parser, TOKEN_TYPE_RPAREN);

    ast_node_t* node = function_call_node_new(callee, arguments_head, count);
    // node = parser_check_and_do_everything(parser, node);
    return node;
}

ast_node_t *parser_parse_function_definition(parser_t *parser)
{
    parameter_t* parameter = parameter_new();
    parameter_node_type_t type = PARAMETER_NODE_TYPE_NORMAL;

    parser_eat(parser, TOKEN_TYPE_LPAREN);

    if (!parser_match(parser, TOKEN_TYPE_RPAREN))
    {
        do
        {
            type = PARAMETER_NODE_TYPE_NORMAL;

            if (parser_match(parser, TOKEN_TYPE_ASTERISK))
            {
                parser_eat(parser, TOKEN_TYPE_ASTERISK);
                type = PARAMETER_NODE_TYPE_ARGS;
            }
            parameter_push(parameter, parser_eat(parser, TOKEN_TYPE_IDENTIFIER).value.as_string_view, type);

            if (!parser_match(parser, TOKEN_TYPE_COMMA)) break;
            parser_eat(parser, TOKEN_TYPE_COMMA);
        } while (1);
    }

    parser_eat(parser, TOKEN_TYPE_RPAREN);

    ast_node_t *body = parser_parse_statement(parser);
    ast_node_t *node = function_definition_node_new(parameter, body);

    // node = parser_check_and_do_everything(parser, node);

    return node;
}

ast_node_t *parser_parse_structure_definition(parser_t *parser)
{
    ast_node_t* fields = statement_sequence_node_new(true);

    parser_eat(parser, TOKEN_TYPE_LBRACE);

    while (!parser_match(parser, TOKEN_TYPE_RBRACE))
    {
        bool constant;

        const token_t keyword = parser_eat(parser, TOKEN_TYPE_KEYWORD);

        if (string_view_equals_cstr(keyword.value.as_string_view, "mutab")) constant = false;
        else if (string_view_equals_cstr(keyword.value.as_string_view, "const")) constant = true;
        else THROW("Unexpected field definition keyword at line %llu", parser->current_token.line);

        statement_sequence_node_push(fields->node.statement_sequence, parser_parse_variable_definition(parser, constant));
        parser_eat(parser, TOKEN_TYPE_SEMICOLON);
    }

    parser_eat(parser, TOKEN_TYPE_RBRACE);

    ast_node_t* node = structure_definition_node_new(fields);
    return node;
}

ast_node_t *parser_parse_variable_field_access(parser_t *parser, ast_node_t* object)
{
    parser_eat(parser, TOKEN_TYPE_DOT);

    const token_t field = parser_eat(parser, TOKEN_TYPE_IDENTIFIER);

    ast_node_t *node = variable_field_access_node_new(object, field.value.as_string_view);

    node = parser_check_and_do_everything(parser, node);

    return node;
}

ast_node_t *parser_parse_array_definition(parser_t *parser)
{
    parser_eat(parser, TOKEN_TYPE_LBRACKET);

    ast_node_t *statement_sequence = statement_sequence_node_new(true);

    if (!parser_match(parser, TOKEN_TYPE_RBRACKET))
    {
        statement_sequence_node_push(statement_sequence->node.statement_sequence, parser_parse_statement(parser));

        while (parser_match(parser, TOKEN_TYPE_COMMA))
        {
            parser_eat(parser, TOKEN_TYPE_COMMA);
            statement_sequence_node_push(statement_sequence->node.statement_sequence, parser_parse_statement(parser));
        }
    }
    parser_eat(parser, TOKEN_TYPE_RBRACKET);

    ast_node_t *node = array_definition_node_new(statement_sequence);
    // node = parser_check_and_do_everything(parser, node);
    return node;
}

ast_node_t *parser_parse_array_access(parser_t *parser, ast_node_t* array)
{
    parser_eat(parser, TOKEN_TYPE_LBRACKET);
    ast_node_t* index = parser_parse_statement(parser);
    parser_eat(parser, TOKEN_TYPE_RBRACKET);


    ast_node_t *node = array_access_node_new(array, index);
    //node = parser_check_and_do_everything(parser, node);
    return node;
}

ast_node_t *parser_parse_if(parser_t *parser)
{
    ast_node_t *condition = parser_parse_statement(parser);

    ast_node_t *if_body = parser_parse_statement(parser);
    ast_node_t *else_body = nullptr;

    if (parser_match(parser, TOKEN_TYPE_KEYWORD) &&
        string_view_equals_cstr(parser->current_token.value.as_string_view, "else"))
    {
        parser_eat(parser, TOKEN_TYPE_KEYWORD);
        else_body = parser_parse_statement(parser);
    }

    return condition_node_new(condition, if_body, else_body);
}

ast_node_t *parser_parse_while(parser_t *parser)
{
    ast_node_t *condition = parser_parse_statement(parser);

    ast_node_t *while_body = parser_parse_statement(parser);
    ast_node_t *else_body = nullptr;

    if (parser_match(parser, TOKEN_TYPE_KEYWORD) &&
        string_view_equals_cstr(parser->current_token.value.as_string_view, "else"))
    {
        parser_eat(parser, TOKEN_TYPE_KEYWORD);
        else_body = parser_parse_statement(parser);
    }

    return while_loop_node_new(condition, while_body, else_body);
}

ast_node_t *parser_parse_for(parser_t *parser)
{
    const string_view_t iterator = parser_eat(parser, TOKEN_TYPE_IDENTIFIER).value.as_string_view;

    if (parser_match(parser, TOKEN_TYPE_KEYWORD))
    {
        if (!string_view_equals_cstr(parser_eat(parser, TOKEN_TYPE_KEYWORD).value.as_string_view, "in"))
            THROW("Unexpected keyword to for loop");
    }

    ast_node_t *iterable = parser_parse_statement(parser);

    ast_node_t *for_body = parser_parse_statement(parser);
    ast_node_t *else_body = nullptr;

    if (parser_match(parser, TOKEN_TYPE_KEYWORD) &&
        string_view_equals_cstr(parser->current_token.value.as_string_view, "else"))
    {
        parser_eat(parser, TOKEN_TYPE_KEYWORD);
        else_body = parser_parse_statement(parser);
    }

    return for_loop_node_new(iterator, iterable, for_body, else_body);
}

ast_node_t *parser_parse_break(parser_t *parser)
{
    ast_node_t* break_body = nullptr;

    if (!parser_match(parser, TOKEN_TYPE_SEMICOLON))
    {
        break_body = parser_parse_statement(parser);
    }

    return break_node_new(break_body);
}

ast_node_t *parser_parse_import(parser_t *parser)
{
    ast_node_t* source = parser_parse_statement(parser);

    return import_node_new(source);
}

ast_node_t *parser_parse_throw(parser_t *parser)
{
    ast_node_t* value = parser_parse_statement(parser);

    return throw_node_new(value);
}

ast_node_t* parser_parse_statement(parser_t* parser)
{
    return parser_parse_comparison(parser);
}

ast_node_t* parser_parse_comparison(parser_t* parser)
{
    ast_node_t *left = parser_parse_expression(parser);
    ast_node_t *right = nullptr;

    while (parser_match(parser, TOKEN_TYPE_EQUALS_EQUALS) ||
        parser_match(parser, TOKEN_TYPE_LESS_EQUALS) ||
        parser_match(parser, TOKEN_TYPE_LESS) ||
        parser_match(parser, TOKEN_TYPE_GREATER_EQUALS) ||
        parser_match(parser, TOKEN_TYPE_GREATER))
    {
        const token_t operation = parser_eat(parser, parser->current_token.type);

        right = parser_parse_expression(parser);
        left = binary_op_node_new(operation, left, right);
    }

    return left;
}

ast_node_t *parser_parse_expression(parser_t *parser)
{
    ast_node_t *left = parser_parse_term(parser);
    ast_node_t *right = nullptr;

    while (parser_match(parser, TOKEN_TYPE_PLUS) ||
           parser_match(parser, TOKEN_TYPE_MINUS))
    {
        const token_t operation = parser_eat(parser, parser->current_token.type);

        right = parser_parse_term(parser);
        left = binary_op_node_new(operation, left, right);
    }

    return left;
}

ast_node_t *parser_parse_term(parser_t *parser)
{
    ast_node_t *left = parser_parse_power(parser);
    ast_node_t *right = nullptr;

    while (parser_match(parser, TOKEN_TYPE_ASTERISK) ||
        parser_match(parser, TOKEN_TYPE_SLASH) ||
        parser_match(parser, TOKEN_TYPE_SLASH_SLASH))
    {
        const token_t operation = parser_eat(parser, parser->current_token.type);

        right = parser_parse_power(parser);
        left = binary_op_node_new(operation, left, right);
    }

    return left;
}

ast_node_t *parser_parse_power(parser_t *parser)
{
    ast_node_t *left = parser_parse_factor(parser);
    ast_node_t *right = nullptr;

    while (parser_match(parser, TOKEN_TYPE_ASTERISK_ASTERISK))
    {
        const token_t operation = parser_eat(parser, parser->current_token.type);

        right = parser_parse_factor(parser);
        left = binary_op_node_new(operation, left, right);
    }

    return left;
}

ast_node_t *parser_parse_factor(parser_t *parser)
{
    if (parser_match(parser, TOKEN_TYPE_MINUS) ||
        parser_match(parser, TOKEN_TYPE_PLUS) ||
        parser_match(parser, TOKEN_TYPE_EXCLAMATION_MARK))
    {
        const token_t op = parser_eat(parser, parser->current_token.type);
        ast_node_t *operand = parser_parse_factor(parser);
        return unary_op_node_new(op, operand);
    }

    ast_node_t *node = nullptr;

    switch (parser->current_token.type)
    {
        case TOKEN_TYPE_NUMBER:
            node = literal_node_new(value_new_number(parser_eat(parser, TOKEN_TYPE_NUMBER).value.as_number));
            break;
        case TOKEN_TYPE_STRING:
            node = literal_node_new(value_new_string(parser_eat(parser, TOKEN_TYPE_STRING).value.as_string));
            break;
        case TOKEN_TYPE_IDENTIFIER:
            node = parser_parse_identifier(parser);
            break;
        case TOKEN_TYPE_KEYWORD:
            node = parser_parse_keyword(parser);
            break;
        case TOKEN_TYPE_LBRACKET:
            node = parser_parse_array_definition(parser);
            break;
        case TOKEN_TYPE_LPAREN:
            parser_eat(parser, TOKEN_TYPE_LPAREN);
            node = parser_parse_statement(parser);
            parser_eat(parser, TOKEN_TYPE_RPAREN);
            break;
        case TOKEN_TYPE_LBRACE:
            node = parser_parse_statement_sequence(parser, true);
            break;
        default:
            break;
    }

    node = parser_check_and_do_everything(parser, node);

    if (!node)
    {
        THROW("unexpected token type '%d' at line %llu", parser->current_token.type, parser->current_token.line);
    }

    return node;
}

void parser_free(parser_t *parser)
{
    free(parser);
}
