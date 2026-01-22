#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "a89alloc.h"


// ===========================
// CRIAÇÃO DE NODES DA AST
// ===========================
static ASTNode* node_alloc(void)
{
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    if (!node) {
        printf("Erro ao alocar memória para node da AST\n");
        exit(EXIT_FAILURE);
    }
    return node; 
}

ASTNode* create_number_node(double number_value, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_NUMBER;
    node->value.number_value = number_value;
    node->value.operator_char = '\0'; 
    node->left = node->right = node->operand = NULL;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_BINARY_OP;
    node->value.number_value = 0;
    node->value.operator_char = operator;
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

    node->value.number_value = 0;
    node->value.operator_char = operator;
    node->left = node->right = NULL;
    node->operand = operand;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_identifier_node(const char* name, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_IDENTIFIER;
    strncpy(node->value.identifier_name, name, MAX_IDENTIFIER_LEN);
    node->value.identifier_name[MAX_IDENTIFIER_LEN] = '\0';
    node->left = node->right = node->operand = node->next = NULL;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_assignment_node(const char* name, ASTNode* value, int line, int column) {
    ASTNode* node = node_alloc();
    node->type = NODE_ASSIGNMENT;
    strncpy(node->value.identifier_name, name, MAX_IDENTIFIER_LEN);
    node->value.identifier_name[MAX_IDENTIFIER_LEN] = '\0';
    node->operand = value;// VALOR DA ATRIBUIÇÃO - operand  
    node->left = node->right = node->next = NULL;
    node->line = line;
    node->column = column;
    return node;
}

ASTNode* create_program_node(ASTNode* first_statement) {
    ASTNode* node = node_alloc();
    node->type = NODE_PROGRAM;
    node->left = first_statement; // PRIMEIRA STATEMENT - left
    node->right = node->operand = node->next = NULL;
    node->line = 1;
    node->column = 1;
    return node;
}

// ===========================
// LIBERANDO A MEMÓRIA ALOCADA
// ===========================
void free_ast(ASTNode* node)
{
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->operand); 
    free_ast(node->next);// PARA LIBERAR A LISTA ENCADEADA    
    a89free(node);
}

void print_ast(ASTNode* node, int indent)
{
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("    ");
    
    switch (node->type)
    {
        case NODE_NUMBER:
            printf("NUMBER: %.2f\n", node->value.number_value);
            break;
        case NODE_BINARY_OP:
            printf("BINARY_OP: %c\n", node->value.operator_char);
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
        case NODE_UNARY_OP:
            printf("UNARY_OP: %c\n", node->value.operator_char);
            print_ast(node->operand, indent + 1);
            break;
        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->value.identifier_name);
            break;
        case NODE_ASSIGNMENT:
            printf("ASSIGNMENT: %s =\n", node->value.identifier_name);
            print_ast(node->operand, indent + 1);
            break;
        case NODE_PROGRAM:
        {
            printf("PROGRAM:\n");
            // Imprime a lista encadeada de statements
            ASTNode* stmt = node->left;
            while (stmt != NULL)
            {
                print_ast(stmt, indent + 1);
                stmt = stmt->next;
            }
            a89free(stmt);
        }
            break;
    }
}
