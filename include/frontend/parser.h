#ifndef PESEC_PARSER_H
#define PESEC_PARSER_H

#include "include/ast/node.h"
#include "include/frontend/lexer.h"

typedef struct
{
    pesec_lexer_t* lexer;
    pesec_token_t token;
} pesec_parser_t;

pesec_parser_t pesec_parser_new(pesec_lexer_t* lexer);

pesec_node_t* pesec_parser_parse(pesec_parser_t* parser);

#endif // PESEC_PARSER_H