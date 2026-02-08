/*
 * ========================================================
 * TESTE AST v0.5.3 - loop while, break, continue
 * Simples e direto - testando apenas o que existe
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "utils.h"
#include "ast.h"

int main(void)
{
    setup_utf8();
    printf("ZzBasic AST Test v0.5.3 - loop while, break, continue\n\n");
    
    // ========================================================
    // TESTE 1: WHILE simples com número
    // ========================================================
    printf("=== TESTE 1: WHILE simples (numero) ===\n");
    ASTNode* condition1 = create_number_node(10, 1, 1);
    ASTNode* body1 = create_statement_list_node(1, 1);
    ASTNode* while_node1 = create_while_node(condition1, body1, 1, 1);
    print_ast(while_node1, 0);
    printf("\n");
    free_ast(while_node1);
    
    // ========================================================
    // TESTE 2: WHILE com booleano
    // ========================================================
    printf("=== TESTE 2: WHILE com booleano (true) ===\n");
    ASTNode* condition2 = create_bool_node(1, 2, 1);
    ASTNode* body2 = create_statement_list_node(2, 1);
    ASTNode* while_node2 = create_while_node(condition2, body2, 2, 1);
    print_ast(while_node2, 0);
    printf("\n");
    free_ast(while_node2);
    
    // ========================================================
    // TESTE 3: WHILE com comparação
    // ========================================================
    printf("=== TESTE 3: WHILE com comparacao (x < 10) ===\n");
    ASTNode* var_x = create_variable_node("x", 3, 1);
    ASTNode* num_10 = create_number_node(10, 3, 5);
    ASTNode* condition3 = create_comparison_op_node(OP_LESS, var_x, num_10, 3, 3);
    ASTNode* body3 = create_statement_list_node(3, 1);
    ASTNode* while_node3 = create_while_node(condition3, body3, 3, 1);
    print_ast(while_node3, 0);
    printf("\n");
    free_ast(while_node3);
    
    // ========================================================
    // TESTE 4: BREAK
    // ========================================================
    printf("=== TESTE 4: BREAK ===\n");
    ASTNode* break_node = create_break_node(4, 5);
    print_ast(break_node, 0);
    printf("\n");
    free_ast(break_node);
    
    // ========================================================
    // TESTE 5: CONTINUE
    // ========================================================
    printf("=== TESTE 5: CONTINUE ===\n");
    ASTNode* continue_node = create_continue_node(5, 5);
    print_ast(continue_node, 0);
    printf("\n");
    free_ast(continue_node);
    
    printf("Teste concluído.\n");
    a89check_leaks();
    return 0;
}
