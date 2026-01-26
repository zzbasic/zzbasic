#include <stdio.h>
#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main() {
    setup_utf8();
    
    printf("%s=== TESTE PARSER PRINT v0.4.0 ===%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    char* testes[] = {
        // Testes básicos
        "print \"Olá Mundo\"",
        "print 42",
        "print x",
        
        // Com nl
        "print \"Texto\" nl",
        "print 10 + 20 nl",
        
        // Múltiplos itens
        "print \"Nome:\" nome \"Idade:\" idade",
        "print \"Soma:\" 10 + 20 \"=\" 30",
        
        // Atalho ?
        "? \"Teste rápido\"",
        "? 3.14 nl",
        
        // Sem nl (acumula)
        "print \"A\" print \"B\" print \"C\" nl",
        
        // Com variáveis e expressões
        "print \"Dobro de 5:\" 5 * 2 nl",
        
        // Print vazio
        "print",
        "print nl",
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++) {
        printf("%sTeste %d: '%s'%s\n", COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        ASTNode* ast = parse(&lexer);
        
        if (ast) {
            printf("AST gerada:\n");
            print_ast(ast, 0);
            
            // Verifica se é um print node
            if (ast->type == NODE_PRINT) {
                printf("✓ Nó PRINT criado\n");
            } else if (ast->type == NODE_STATEMENT_LIST) {
                printf("✓ Lista de statements (pode conter PRINT)\n");
            }
            
            free_ast(ast);
            printf("%s✓ Parsing OK%s\n", COLOR_SUCCESS, COLOR_RESET);
        } else {
            printf("%s✗ ERRO no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        printf("\n");
        wait();  // <--- ADICIONADO AQUI
    }
    
    // Teste com múltiplos statements separados por ;
    printf("%sTeste multi-statements: 'print \"A\"; print \"B\" nl'%s\n", 
           COLOR_HEADER, COLOR_RESET);
    
    Lexer lexer;
    lexer_init(&lexer, "print \"A\"; print \"B\" nl");
    ASTNode* ast = parse(&lexer);
    
    if (ast) {
        printf("AST:\n");
        print_ast(ast, 0);
        free_ast(ast);
        printf("%s✓ Funcionou com ; separador%s\n", COLOR_SUCCESS, COLOR_RESET);
    } else {
        printf("%s✗ Problema com ; separador%s\n", COLOR_ERROR, COLOR_RESET);
    }
    
    wait();  // <--- ADICIONADO AQUI
    
    printf("\n%s=== TESTES COMPLETADOS ===%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    return 0;
}