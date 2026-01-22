#ifndef AST_H
#define AST_H

#include "zzdefs.h"

typedef enum {
    NODE_NULL,
    NODE_NUMBER,
    NODE_BINARY_OP,     //operacao binaria (+, -, *, /)
    NODE_UNARY_OP,      //operacao unaria (-)
    NODE_IDENTIFIER,    // identificador (nome de variável) 
    NODE_ASSIGNMENT,    // atribuição LET x = 5 
    NODE_PROGRAM    
} Node_type;

typedef struct ASTNode {
    Node_type type;
    
    // Dados especificos do no
    //double value;    // Para NODE_NUMBER 
    //char operator;   // Para NODE_BINARY_OP e NODE_UNARY_OP

    // Dados especificos do no
    union {
        double number_value;                            // Para NODE_NUMBER
        char identifier_name[MAX_IDENTIFIER_LEN + 1];   // Para NODE_IDENTIFIER
        char operator_char;                             // Para operações
    } value;
    
    // Filhos do no
    struct ASTNode* left;       // Filho esquerdo (operacoes binarias)
    struct ASTNode* right;      // Filho direito (operacoes binarias)
    struct ASTNode* operand;    // Operando (operacoes unarias e atribuição) 
    struct ASTNode* next;       // Próximo statement (para programas)

    int line;   // Linha do início do token
    int column; // Coluna do início do token
} ASTNode;

// v0.1.0
ASTNode* create_number_node(double value, int line, int column);
ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, int line, int column);
ASTNode* create_unary_op_node(char operator, ASTNode* operand, int line, int column);

// v0.2.0
ASTNode* create_identifier_node(const char* name, int line, int column);
ASTNode* create_assignment_node(const char* name, ASTNode* value, int line, int column);
ASTNode* create_program_node(ASTNode* first_statement);

void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif // AST_H