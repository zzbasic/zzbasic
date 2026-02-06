// parser.h

#ifndef PARSER_H
// parser.h

#define PARSER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zzdefs.h"
#include "lexer.h"
#include "ast.h"
#include "color_mapping.h"

typedef struct Parser{
    Lexer* lexer;
    Token current_token;
    int has_error;
    char error_message[BUFFER_SIZE];
} Parser;

ASTNode* parse(Lexer* lexer);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

ASTNode* parse_single_statement(Lexer* lexer);

#endif // PARSER_H
// Fim de parser.h