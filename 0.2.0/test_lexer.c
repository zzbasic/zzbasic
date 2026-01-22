#include <stdio.h>

#include "color.h"
#include "lexer.h"
#include "utils.h"

int main()
{
    setup_utf8();
    
    printf("Teste do Lexer ZzBasic v0.2.0\n\n");
    
    // ============================================
    // TESTE 1: Expressões matemáticas básicas
    // ============================================
    printf("%s1. Expressões matemáticas:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("2 + 3 * 4");
    wait();
    
    // ============================================
    // TESTE 2: Números decimais e parênteses
    // ============================================
    printf("\n%s2. Números decimais e parênteses:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("(10.5 - 2) / 3.14");
    wait();
    
    // ============================================
    // TESTE 3: Comentários
    // ============================================
    printf("\n%s3. Comentários:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("# Comentário no início\n5 + 3 # comentário no final");
    wait();
    
    // ============================================
    // TESTE 4: Declaração LET e identificadores
    // ============================================
    printf("\n%s4. Declaração LET e identificadores:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("let x = 10\nlet y = x + 5\nz = x * y");
    wait();
    
    // ============================================
    // TESTE 5: Case-sensitive (let vs LET vs Let)
    // ============================================
    printf("\n%s5. Case-sensitive (minúsculas):%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("let x = 5\nLet y = 10\nLET z = 15");
    wait();
    
    // ============================================
    // TESTE 6: Operador unário negativo
    // ============================================
    printf("\n%s6. Operador unário negativo:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("-5 + 3\n-(10 * 2)\n5 * -2");
    wait();
    
    // ============================================
    // TESTE 7: Identificadores válidos
    // ============================================
    printf("\n%s7. Identificadores válidos:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("x _var variavel123 total_geral a_b_c");
    wait();
    
    // ============================================
    // TESTE 8: Números inválidos (teste de erro)
    // ============================================
    printf("\n%s8. Números inválidos:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("12.34.56");
    wait();
    lexer_print_all_tokens("12.");
    wait();
    lexer_print_all_tokens("12..34");
    wait();
    
    // ============================================
    // TESTE 9: Limite de identificador (63 chars)
    // ============================================
    printf("\n%s9. Limite de identificador (63 chars):%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("var_com_63_caracteres_abcdefghijklmnopqrstuvwxyz0123456789_abc");
    wait();
    
    // ============================================
    // TESTE 10: Mix completo
    // ============================================
    printf("\n%s10. Expressão completa:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("let total = (preco * quantidade) - desconto # cálculo do total");
    
    printf("\n%s✅ Todos os testes completados!%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    return 0;
}