#ifndef AST_H
#define AST_H

typedef enum {
    NODE_NULL,
    NODE_NUMBER,
    NODE_BINARY_OP, //operacao binaria (+, -, *, /)
    NODE_UNARY_OP,  //operacao unaria (-)
} Node_type;

typedef struct ASTNode {
    Node_type type;
    
    // Dados especificos do no
    double value;    // Para NODE_NUMBER 
    char operator;   // Para NODE_BINARY_OP e NODE_UNARY_OP
    
    // Filhos do no
    struct ASTNode* left;   // Filho esquerdo (operacoes binarias)
    struct ASTNode* right;  // Filho direito (operacoes binarias)
    struct ASTNode* operand;// Operando (operacoes unarias) 

    int line;   // Linha do início do token
    int column; // Coluna do início do token
} ASTNode;

ASTNode* create_number_node(double value, int line, int column);
ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, int line, int column);
ASTNode* create_unary_op_node(char operator, ASTNode* operand, int line, int column);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif // AST_H