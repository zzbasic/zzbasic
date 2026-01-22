#ifndef PARSER_H
#define PARSER_H

#include "zzdefs.h"
#include "ast.h"
#include "lexer.h"

typedef struct {
    Lexer* lexer;
    Token current_token;
    int has_error;
    char error_message[BUFFER_SIZE];
    int line;   
    int column; 
} Parser;

// ============================================
// Interface Pública do Parser
// ============================================
//ASTNode* parse(Lexer* lexer);
ASTNode* parse_program(Lexer* lexer);

#endif // PARSER_H
// fim de parser.h