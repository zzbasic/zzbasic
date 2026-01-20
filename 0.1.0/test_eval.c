#include <stdio.h>
#include <string.h>

#include "color.h"
#include "utils.h"
#include "a89alloc.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "evaluator.h"

void test_expression(const char* expr, double expected) {
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sExpressão: %s%s\n", YELLOW, expr, COLOR_RESET);
    printf("%sEsperado: %.2f%s\n", COLOR_INFO, expected, COLOR_RESET);
    
    Lexer lexer;
    lexer_init(&lexer, expr);
    
    ASTNode* ast = parse(&lexer);
    if (ast == NULL) {
        printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        return;
    }
    
    printf("%sÁrvore gerada:%s\n", COLOR_INFO, COLOR_RESET);
    print_ast(ast, 1);
    
    EvaluatorResult result = evaluate(ast);
    
    if (result.success) {
        printf("%sResultado: %.6f%s\n", COLOR_SUCCESS, result.value.number_value, COLOR_RESET);
        
        // Verifica se bate com o esperado (com tolerância para ponto flutuante)
        double diff = result.value.number_value - expected;
        if (diff < 0) diff = -diff;
        
        if (diff < 0.000001) {
            printf("%s✓ Resultado CORRETO!%s\n", COLOR_SUCCESS, COLOR_RESET);
        } else {
            printf("%s✗ Resultado INCORRETO! Diferença: %.6f%s\n", 
                   COLOR_ERROR, diff, COLOR_RESET);
        }
    } else {
        printf("%s✗ Erro na avaliação: %s%s\n", 
               COLOR_ERROR, result.error_message, COLOR_RESET);
    }
    
    free_ast(ast);

    wait();
}

void test_error_cases(void) {
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sTESTES DE ERRO%s\n", CYAN, COLOR_RESET);
    
    const char* error_expressions[] = {
        "5 / 0",           // Divisão por zero
        "5 + ",            // Expressão incompleta
        "(5 + 3",          // Parêntese faltando
        "12.34.56",        // Número inválido
    };
    
    int num_errors = sizeof(error_expressions) / sizeof(error_expressions[0]);
    
    for (int i = 0; i < num_errors; i++) {
        printf("\n%sTeste %d: %s%s\n", YELLOW, i+1, error_expressions[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, error_expressions[i]);
        
        ASTNode* ast = parse(&lexer);
        if (ast == NULL) {
            printf("%s✓ Parsing falhou como esperado%s\n", COLOR_SUCCESS, COLOR_RESET);
            continue;
        }
        
        EvaluatorResult result = evaluate(ast);
        if (!result.success) {
            printf("%s✓ Avaliação falhou: %s%s\n", 
                   COLOR_SUCCESS, result.error_message, COLOR_RESET);
        } else {
            printf("%s✗ FALHA: Avaliação deveria ter falhado%s\n", 
                   COLOR_ERROR, COLOR_RESET);
        }
        
        free_ast(ast);
    }

    wait();
}

int main(void) {
    setup_utf8();
    
    printf("%s┌───────────────────────────────────────┐%s\n", CYAN, COLOR_RESET);
    printf("%s│   AVALIADOR ZzBasic v0.1.0            │%s\n", CYAN, COLOR_RESET);
    printf("%s└───────────────────────────────────────┘%s\n\n", CYAN, COLOR_RESET);
    
    // Testes que devem funcionar
    printf("%sTESTES DE SUCESSO%s\n", CYAN, COLOR_RESET);
    
    test_expression("5 + 3", 8.0);
    test_expression("10 - 4 * 2", 2.0);
    test_expression("(5 + 3) * 2", 16.0);
    test_expression("10 / 2 + 1", 6.0);
    test_expression("-5 + 3", -2.0);
    test_expression("-(5 + 3)", -8.0);
    test_expression("5 * -2", -10.0);
    test_expression("+5", 5.0);
    test_expression("3.14 * 2", 6.28);
    test_expression("10 / 3", 3.333333);
    
    // Testes de erro
    test_error_cases();
    
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sVerificação de vazamento de memória:%s\n", YELLOW, COLOR_RESET);
    a89check_leaks();
    
    return 0;
}