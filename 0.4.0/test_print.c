#include <stdio.h>
#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "evaluator.h"
#include "a89alloc.h"

int main() {
    setup_utf8();
    
    printf("%s=== TESTE EVALUATOR PRINT v0.4.0 ===%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    SymbolTable* symbols = symbol_table_create();
    
    // Testes básicos
    char* testes[] = {
        // Básico
        "print \"Olá Mundo\" nl",
        "print 42 nl",
        "print 3.14159 nl",
        
        // Múltiplos itens
        "print \"A\" \"B\" \"C\" nl",
        "print \"Nome:\" \"Zurg\" nl",
        
        // Com variáveis
        "let x = 10",
        "print \"Valor de x:\" x nl",
        
        // Expressões
        "print \"Soma:\" 10 + 20 nl",
        "let a = 5\nlet b = 3\nprint a \"*\" b \"=\" a * b nl",
        
        // Sem nl (acumula)
        "print \"Linha \"",
        "print \"continua\" nl",
        
        // Print vazio
        "print nl",
        "print",  // Deve ser equivalente a print nl
        
        // Números formatados
        "print 10.0 nl",     // Deve mostrar 10
        "print 10.500 nl",   // Deve mostrar 10.5
        "print 10.000 nl",   // Deve mostrar 10
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++) {
        printf("\n%sTeste %d: '%s'%s\n", COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        ASTNode* ast = parse(&lexer);
        
        if (ast) {
            printf("Executando...\n");
            int success = evaluate_program(ast, symbols);
            
            if (success) {
                printf("%s✓ Sucesso%s\n", COLOR_SUCCESS, COLOR_RESET);
            } else {
                printf("%s✗ Erro na execução%s\n", COLOR_ERROR, COLOR_RESET);
            }
            
            free_ast(ast);
        } else {
            printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        wait();
    }
    
    // Mostra estado final das variáveis
    printf("\n%sVariáveis após testes:%s\n", COLOR_HEADER, COLOR_RESET);
    symbol_table_print(symbols);
    
    symbol_table_destroy(symbols);
    
    printf("\n%s=== TESTES COMPLETADOS ===%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    a89check_leaks();
    return 0;
}