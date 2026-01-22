#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    Lexer* lexer;
    Token current_token;
    int has_error;
    char error_message[BUFFER_SIZE];
    // Posição atual no parsing (para mensagens de erro)
    int line;   // Linha do início do token
    int column; // Coluna do início do token
} Parser;

// ============================================
// Interface Pública do Parser
// ============================================

// Funcao principal de parsing
ASTNode* parse(Lexer* lexer);

#endif // PARSER_H
// fim de parser.h