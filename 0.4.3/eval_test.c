// eval_test.c

#include <stdio.h>
#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#include "evaluator.h"
#include "a89alloc.h"


void test_width_alignment(void) {
    printf("%s=== TESTE WIDTH E ALINHAMENTO (v0.4.2) ===%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    const char* testes[] = {
        // Teste 1: Width básico
        "print width(10) \"Teste\" nl",
        
        // Teste 2: Alinhamento left
        "print width(10) left \"Left\" nl",
        
        // Teste 3: Alinhamento right  
        "print width(10) right \"Right\" nl",
        
        // Teste 4: Alinhamento center
        "print width(10) center \"Center\" nl",
        
        // Teste 5: Sequência de formatações
        "print width(10) \"A\" width(15) \"B\" width(20) \"C\" nl",
        
        // Teste 6: Com números
        "print width(10) 123 width(15) 45.6 nl",
        
        // Teste 7: Com variáveis
        "let nome = \"Zz\"\nprint width(20) center nome nl",
        
        // Teste 8: Mix de formatação
        "print width(10) left \"Nome:\" width(20) right \"João\" nl",
        
        // Teste 9: String maior que width
        "print width(5) \"TextoLongo\" nl",
        
        // Teste 10: Com cores
        "print red width(10) \"ERRO\" nocolor width(15) green \"OK\" nocolor nl",
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++) {
        printf("%s\nTeste %d:%s\n", CYAN, i+1, COLOR_RESET);
        printf("Código: %s\"%s\"%s\n", YELLOW, testes[i], COLOR_RESET);
        printf("%sSaída:%s ", GREEN, COLOR_RESET);
        
        // Setup
        SymbolTable* symbols = symbol_table_create();
        
        // Execução
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        ASTNode* ast = parse(&lexer);
        
        if (ast) {
            int success = evaluate_program(ast, symbols);
            if (!success) {
                printf("%s✗ Erro na execução%s\n", COLOR_ERROR, COLOR_RESET);
            }
            free_ast(ast);
        } else {
            printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        symbol_table_destroy(symbols);
        printf("\n");
        wait();
    }
    
    // Teste especial: tabela formatada
    printf("%s\n=== TABELA FORMATADA ===%s\n", COLOR_HEADER, COLOR_RESET);
    printf("Código:\n");
    printf("%sprint width(10) left \"ID\" width(20) center \"NOME\" width(15) right \"SALDO\" nl\n", YELLOW);
    printf("print \"-\" \"-\" \"-\" nl\n");
    printf("print width(10) left 101 width(20) center \"João\" width(15) right 1500.50 nl\n");
    printf("print width(10) left 102 width(20) center \"Maria\" width(15) right 2300.75 nl%s\n", COLOR_RESET);
    
    printf("%sSaída:%s\n", GREEN, COLOR_RESET);
    
    const char* tabela = 
        "print width(10) left \"ID\" width(20) center \"NOME\" width(15) right \"SALDO\" nl\n"
        "print \"-\" \"-\" \"-\" nl\n"
        "print width(10) left 101 width(20) center \"João\" width(15) right 1500.50 nl\n"
        "print width(10) left 102 width(20) center \"Maria\" width(15) right 2300.75 nl";
    
    SymbolTable* symbols = symbol_table_create();
    Lexer lexer;
    lexer_init(&lexer, tabela);
    ASTNode* ast = parse(&lexer);
    
    if (ast) {
        evaluate_program(ast, symbols);
        free_ast(ast);
    }
    
    symbol_table_destroy(symbols);
    
    printf("\n%s✅ Testes de width/alinhamento concluídos!%s\n", COLOR_SUCCESS, COLOR_RESET);
}

int main(void) {
    setup_utf8();
    test_width_alignment();
    
    // Verificação de memória
    printf("\n%sVerificando vazamentos de memória:%s\n", CYAN, COLOR_RESET);
    a89check_leaks();
    
    return 0;
    
}
// Fim de eval_test.c