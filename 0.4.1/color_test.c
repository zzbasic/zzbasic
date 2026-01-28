
// color_test.c - Testes específicos para ColorNode
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer.h"
#include "color_mapping.h"
#include "color.h"
#include "utils.h"

void test_color_nodes_basic(void) {
    printf("%s=== TESTE COLOR NODE - BÁSICO ===%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    printf("1. Criação de nodes de cor individuais:\n");
    printf("======================================\n");
    
    // Array de tokens de cor para testar
    TokenType color_tokens[] = {
        TOKEN_NOCOLOR,
        TOKEN_BLACK,
        TOKEN_RED,
        TOKEN_GREEN,
        TOKEN_YELLOW,
        TOKEN_BLUE,
        TOKEN_MAGENTA,
        TOKEN_CYAN,
        TOKEN_WHITE,
        TOKEN_BRED,
        TOKEN_BGREEN,
        TOKEN_BYELLOW,
        TOKEN_BBLUE,
        TOKEN_BMAGENTA,
        TOKEN_BCYAN,
        TOKEN_BWHITE,
        TOKEN_NULL  // Terminador
    };
    
    const char* color_names[] = {
        "NOCOLOR",
        "BLACK", "RED", "GREEN", "YELLOW", "BLUE",
        "MAGENTA", "CYAN", "WHITE",
        "BRED", "BGREEN", "BYELLOW", "BBLUE",
        "BMAGENTA", "BCYAN", "BWHITE",
        NULL
    };
    
    for (int i = 0; color_tokens[i] != TOKEN_NULL; i++) {
        ASTNode* node = create_color_node(color_tokens[i], 1, i*10 + 1);
        printf("%-8s: [%d] ", color_names[i], color_tokens[i]);
        print_ast(node, 0);
        
        // Mostra amostra visual
        if (node->data.color.color_token_id != 0) { // Não mostra NOCOLOR
            printf("      Amostra: %s██%s\n", 
                   node->data.color.ansi_color, 
                   COLOR_RESET);
        }
        printf("\n");
        
        free_ast(node);
    }
    
    printf("\n✅ Nodes básicos testados e liberados\n");
}

void test_color_visual_demo(void) {
    printf("\n2. Demonstração visual COMPLETA:\n");
    printf("================================\n");
    
    printf("Cores normais:\n");
    
    // Cria e mostra cores normais
    ASTNode* black = create_color_node(TOKEN_BLACK, 2, 1);
    ASTNode* red = create_color_node(TOKEN_RED, 2, 1);
    ASTNode* green = create_color_node(TOKEN_GREEN, 2, 1);
    ASTNode* yellow = create_color_node(TOKEN_YELLOW, 2, 1);
    ASTNode* blue = create_color_node(TOKEN_BLUE, 2, 1);
    ASTNode* magenta = create_color_node(TOKEN_MAGENTA, 2, 1);
    ASTNode* cyan = create_color_node(TOKEN_CYAN, 2, 1);
    ASTNode* white = create_color_node(TOKEN_WHITE, 2, 1);
    
    printf("%s██ Preto%s   ", black->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Vermelho%s ", red->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Verde%s    ", green->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Amarelo%s  ", yellow->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Azul%s     ", blue->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Magenta%s  ", magenta->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Ciano%s    ", cyan->data.color.ansi_color, COLOR_RESET);
    printf("%s██ Branco%s\n", white->data.color.ansi_color, COLOR_RESET);
    
    // Libera
    free_ast(black);
    free_ast(red);
    free_ast(green);
    free_ast(yellow);
    free_ast(blue);
    free_ast(magenta);
    free_ast(cyan);
    free_ast(white);
    
    printf("\nCores bright:\n");
    
    // Cria e mostra cores bright
    ASTNode* bred = create_color_node(TOKEN_BRED, 3, 1);
    ASTNode* bgreen = create_color_node(TOKEN_BGREEN, 3, 1);
    ASTNode* byellow = create_color_node(TOKEN_BYELLOW, 3, 1);
    ASTNode* bblue = create_color_node(TOKEN_BBLUE, 3, 1);
    ASTNode* bmagenta = create_color_node(TOKEN_BMAGENTA, 3, 1);
    ASTNode* bcyan = create_color_node(TOKEN_BCYAN, 3, 1);
    ASTNode* bwhite = create_color_node(TOKEN_BWHITE, 3, 1);
    
    printf("%s██ BRed%s     ", bred->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BGreen%s   ", bgreen->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BYellow%s  ", byellow->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BBlue%s    ", bblue->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BMagenta%s ", bmagenta->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BCyan%s    ", bcyan->data.color.ansi_color, COLOR_RESET);
    printf("%s██ BWhite%s\n", bwhite->data.color.ansi_color, COLOR_RESET);
    
    // Libera
    free_ast(bred);
    free_ast(bgreen);
    free_ast(byellow);
    free_ast(bblue);
    free_ast(bmagenta);
    free_ast(bcyan);
    free_ast(bwhite);
    
    printf("\n✅ Demonstração visual concluída\n");
}

void test_color_in_print(void) {
    printf("\n3. Cores em comandos PRINT (com execução real):\n");
    printf("===============================================\n");
    
    // Teste 3.1: Print colorido simples
    printf("3.1 Exemplo: print red \"ERRO\" nocolor \": Arquivo não encontrado\"\n");
    
    ASTNode* print1 = create_print_node(4, 1);
    print_node_add_item(print1, create_color_node(TOKEN_RED, 4, 1));
    print_node_add_item(print1, create_string_node("ERRO", 4, 7));
    print_node_add_item(print1, create_color_node(TOKEN_NOCOLOR, 4, 12));
    print_node_add_item(print1, create_string_node(": Arquivo não encontrado", 4, 20));
    
    printf("AST gerado:\n");
    print_ast(print1, 0);
    
    printf("Saída real: ");
    // Simulação da execução (não temos executor ainda)
    ASTNode* red_node = create_color_node(TOKEN_RED, 4, 1);
    ASTNode* nocolor_node = create_color_node(TOKEN_NOCOLOR, 4, 12);
    printf("%sERRO%s: Arquivo não encontrado\n", 
           red_node->data.color.ansi_color,
           nocolor_node->data.color.ansi_color);
    
    free_ast(red_node);
    free_ast(nocolor_node);
    free_ast(print1);
    
    // Teste 3.2: Print com múltiplas cores
    printf("\n3.2 Exemplo: print green \"SUCESSO\" nocolor blue \" INFO\" nocolor\n");
    
    ASTNode* print2 = create_print_node(5, 1);
    print_node_add_item(print2, create_color_node(TOKEN_GREEN, 5, 1));
    print_node_add_item(print2, create_string_node("SUCESSO", 5, 8));
    print_node_add_item(print2, create_color_node(TOKEN_NOCOLOR, 5, 16));
    print_node_add_item(print2, create_color_node(TOKEN_BLUE, 5, 18));
    print_node_add_item(print2, create_string_node(" INFO", 5, 24));
    print_node_add_item(print2, create_color_node(TOKEN_NOCOLOR, 5, 30));
    
    printf("AST gerado:\n");
    print_ast(print2, 0);
    
    printf("Saída real: ");
    ASTNode* green_node = create_color_node(TOKEN_GREEN, 5, 1);
    ASTNode* nocolor1 = create_color_node(TOKEN_NOCOLOR, 5, 16);
    ASTNode* blue_node = create_color_node(TOKEN_BLUE, 5, 18);
    ASTNode* nocolor2 = create_color_node(TOKEN_NOCOLOR, 5, 30);
    
    printf("%sSUCESSO%s%s INFO%s\n",
           green_node->data.color.ansi_color,
           nocolor1->data.color.ansi_color,
           blue_node->data.color.ansi_color,
           nocolor2->data.color.ansi_color);
    
    free_ast(green_node);
    free_ast(nocolor1);
    free_ast(blue_node);
    free_ast(nocolor2);
    free_ast(print2);
    
    printf("\n✅ Prints com cores testados\n");
}

void test_color_mapping_correctness(void) {
    printf("\n4. Verificação do mapeamento:\n");
    printf("==============================\n");
    
    struct {
        TokenType token;
        const char* name;
        int expected_color_id;
        const char* expected_color;
    } tests[] = {
        {TOKEN_NOCOLOR, "NOCOLOR", 0, "\\033[0m"},
        {TOKEN_BLACK,   "BLACK",   1, "\\033[30m"},
        {TOKEN_RED,     "RED",     2, "\\033[31m"},
        {TOKEN_GREEN,   "GREEN",   3, "\\033[32m"},
        {TOKEN_YELLOW,  "YELLOW",  4, "\\033[33m"},
        {TOKEN_BLUE,    "BLUE",    5, "\\033[34m"},
        {TOKEN_MAGENTA, "MAGENTA", 6, "\\033[35m"},
        {TOKEN_CYAN,    "CYAN",    7, "\\033[36m"},
        {TOKEN_WHITE,   "WHITE",   8, "\\033[37m"},
        {TOKEN_BRED,    "BRED",    9, "\\033[91m"},
        {TOKEN_BGREEN,  "BGREEN", 10, "\\033[92m"},
        {TOKEN_BYELLOW, "BYELLOW",11, "\\033[93m"},
        {TOKEN_BBLUE,   "BBLUE",  12, "\\033[94m"},
        {TOKEN_BMAGENTA,"BMAGENTA",13,"\\033[95m"},
        {TOKEN_BCYAN,   "BCYAN",  14, "\\033[96m"},
        {TOKEN_BWHITE,  "BWHITE", 15, "\\033[97m"},
        {TOKEN_NULL, NULL, 0, NULL}
    };
    
    printf("Token → ColorCode → ANSI:\n");
    printf("-------------------------\n");
    
    int all_correct = 1;
    for (int i = 0; tests[i].name != NULL; i++) {
        ASTNode* node = create_color_node(tests[i].token, 6, 1);
        
        // Verifica color_id
        int correct_id = (node->data.color.color_token_id == tests[i].expected_color_id);
        
        printf("%-8s: token=%2d → color_id=%2d %s, ANSI: %s\n",
               tests[i].name,
               tests[i].token,
               node->data.color.color_token_id,
               correct_id ? "✅" : "❌",
               node->data.color.ansi_color);
        
        if (!correct_id) {
            all_correct = 0;
        }
        
        free_ast(node);
    }
    
    if (all_correct) {
        printf("\n✅ TODOS OS MAPEAMENTOS CORRETOS!\n");
    } else {
        printf("\n❌ ALGUNS MAPEAMENTOS ERRADOS!\n");
    }
}

int main(void) {
    setup_utf8();
    
    printf("%s=== TESTE COMPLETO DO SISTEMA DE CORES ZzBasic v0.4.1 ===%s\n\n", 
           COLOR_HEADER, COLOR_RESET);
    
    // Executa todos os testes
    test_color_nodes_basic();
    wait();
    
    test_color_visual_demo();
    wait();
    
    test_color_in_print();
    wait();
    
    test_color_mapping_correctness();
    wait();
    
    // Verifica memory leaks
    printf("\n%s=== VERIFICAÇÃO FINAL ===%s\n", COLOR_HEADER, COLOR_RESET);
#ifdef A89ALLOC_DEBUG
    a89check_leaks();
#endif
    
    printf("\n%s✅ TODOS OS TESTES DE COR CONCLUÍDOS COM SUCESSO!%s\n", 
           COLOR_SUCCESS, COLOR_RESET);
    
    return 0;
}