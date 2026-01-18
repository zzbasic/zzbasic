#ifndef PARSER_H
#define PARSER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

// ==================================================================
// ÁRVORE SINTÁTICA (AST)
// ==================================================================
typedef enum {
    NODE_NUMBER,
    NODE_BINARY_OP, //operacao binaria (+, -, *, /)
    NODE_UNARY_OP,  //operacao unaria (-)
} Node_type;

typedef struct ASTNode {
    Node_type type;
    
    // Dados especificos do no
    double value;           // Para NODE_NUMBER 
    char operator;          // Para NODE_BINARY_OP e NODE_UNARY_OP
    
    // Filhos do no
    struct ASTNode* left;   // Filho esquerdo (operacoes binarias)
    struct ASTNode* right;  // Filho direito (operacoes binarias)
    struct ASTNode* operand;// Operando (operacoes unarias) 
} ASTNode;

ASTNode* create_number_node(double value);
ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right);
ASTNode* create_unary_op_node(char operator, ASTNode* operand);

void free_ast(ASTNode* node);

// ==================================================================
// PARSER
// ==================================================================
typedef struct {
    Lexer* lexer;
    Token current_token;
    int has_error;
    char error_message[100];
} Parser;

// Inicializa o parser
void parser_init(Parser* parser, Lexer* lexer);

// Avanca para o proximo token
void parser_advance(Parser* parser);

// Verifica se o token atual e do tipo esperado
int parser_expect(Parser* parser, Token_type expected_type);

// Define mensagem de erro no parser
void parser_set_error(Parser* parser, const char* message);

/********************************************************************
FUNCOES DE PARSING 

GRAMATICA v0.1.0

expression       := arithmetic_expr | E
arithmetic_expr  := term (('+' | '-') term)*
term             := factor (('*' | '/') factor)*
factor           := NUMBER | '(' expression ')' |
                    '-' (NUMBER | '(' expression ')')

********************************************************************/
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_arithmetic_expr(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_factor(Parser* parser);

// Funcao principal de parsing
ASTNode* parse(Lexer* lexer);

// Funcao para imprimir a AST (para debug)
void print_ast(ASTNode* node, int indent);

#endif // PARSER_H