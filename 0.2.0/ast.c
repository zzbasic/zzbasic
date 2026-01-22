#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "a89alloc.h"


static ASTNode* node_alloc(void)
{
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    if (!node) {
        printf("Erro ao alocar memória para node da AST\n");
        exit(EXIT_FAILURE);
    }
    return node; 
}

ASTNode* create_number_node(double value, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_NUMBER;
    node->value = value;
    node->operator = '\0'; 
    node->left = node->right = node->operand = NULL;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_BINARY_OP;
    node->value = 0;
    node->operator = operator;
    node->left = left;
    node->right = right;
    node->operand = NULL;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_unary_op_node(char operator, ASTNode* operand, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_UNARY_OP;

    node->value = 0;
    node->operator = operator;
    node->left = node->right = NULL;
    node->operand = operand;
    node->line = line;
    node->column = column;
    return node;
}

// ===========================
// LIBERANDO A MEMÓRIA ALOCADA
// ===========================
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->operand);    
    a89free(node);
}

void print_ast(ASTNode* node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("    ");
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %.2f\n", node->value);
            break;
        case NODE_BINARY_OP:
            printf("BINARY_OP: %c\n", node->operator);
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
        case NODE_UNARY_OP:
            printf("UNARY_OP: %c\n", node->operator);
            print_ast(node->operand, indent + 1);
            break;
    }
}
