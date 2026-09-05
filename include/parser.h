#ifndef PESEC_PARSER_H
#define PESEC_PARSER_H

#include "lexer.h"
#include "ast/ast_node.h"

typedef struct
{
    lexer_t *lexer;
    token_t current_token;
} parser_t;

parser_t *parser_new(lexer_t *lexer);

bool parser_match(const parser_t *parser, token_type_t type);

token_t parser_eat(parser_t *parser, token_type_t type);

ast_node_t *parser_check_and_parse_function_call(parser_t *parser, ast_node_t* callee);

ast_node_t* parser_check_and_parse_variable_field_access(parser_t *parser, ast_node_t* object);

ast_node_t* parser_check_and_parse_variable_assignment(parser_t *parser, ast_node_t* target);

ast_node_t* parser_check_and_parse_variable_complex_assignment(parser_t *parser, ast_node_t* target);

ast_node_t* parser_check_and_parse_vector_access(parser_t *parser, ast_node_t* vector);

ast_node_t* parser_check_and_parse_meta_op(parser_t *parser, ast_node_t* vector);

ast_node_t* parser_check_and_do_everything(parser_t *parser, ast_node_t* node);

ast_node_t *parser_parse_value_meta_op(parser_t *parser, ast_node_t* node);

ast_node_t *parser_parse_keyword(parser_t *parser);

ast_node_t *parser_parse(parser_t *parser);

ast_node_t *parser_parse_statement_sequence(parser_t *parser, bool between_braces);

ast_node_t *parser_parse_identifier(parser_t *parser);

ast_node_t *parser_parse_variable(parser_t *parser, string_view_t name);

ast_node_t *parser_parse_variable_definition(parser_t *parser, bool constant);

ast_node_t *parser_parse_variable_assignment(parser_t *parser, ast_node_t* target);

ast_node_t *parser_parse_variable_complex_assignment(parser_t *parser, ast_node_t* target);

ast_node_t *parser_parse_function_call(parser_t *parser, ast_node_t* callee);

ast_node_t *parser_parse_function_definition(parser_t *parser);

ast_node_t *parser_parse_structure_definition(parser_t *parser);

ast_node_t *parser_parse_variable_field_access(parser_t *parser, ast_node_t* object);

ast_node_t *parser_parse_vector_definition(parser_t *parser);

ast_node_t *parser_parse_vector_access(parser_t *parser, ast_node_t* vector);

ast_node_t *parser_parse_if(parser_t *parser);

ast_node_t *parser_parse_while(parser_t *parser);

ast_node_t *parser_parse_for(parser_t *parser);

ast_node_t *parser_parse_break(parser_t *parser);

ast_node_t *parser_parse_import(parser_t *parser);

ast_node_t *parser_parse_panic(parser_t *parser);

ast_node_t *parser_parse_return(parser_t *parser);

ast_node_t *parser_parse_statement(parser_t *parser);

ast_node_t *parser_parse_or(parser_t *parser);

ast_node_t *parser_parse_and(parser_t *parser);

ast_node_t *parser_parse_comparison(parser_t *parser);

ast_node_t *parser_parse_expression(parser_t *parser);

ast_node_t *parser_parse_term(parser_t *parser);

ast_node_t *parser_parse_power(parser_t *parser);

ast_node_t *parser_parse_factor(parser_t *parser);

void parser_free(parser_t *parser);

#endif // PESEC_PARSER_H
