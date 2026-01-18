#include <stdio.h>

#include "lexer.h"
#include "utils.h"

int main()
{
    setup_utf8();
    
    printf("Teste do Lexer ZzBasic v0.1.0\n\n");
    
    // Teste 1: Expressão simples
    lexer_print_all_tokens("2 + 3 * 4");
    
    printf("\n");
    wait();
    
    // Teste 2: Expressão com parênteses e decimais
    lexer_print_all_tokens("(10.5 - 2) / 3.14");
    
    printf("\n");
    wait();
    
    // Teste 3: Número inválido (para ver erro)
    lexer_print_all_tokens("12.34.56");
    wait();
    
    // Testar outros casos de erro
    lexer_print_all_tokens("12.");wait();      // Ponto sem dígitos após
    lexer_print_all_tokens("12..34");wait();   // Dois pontos consecutivos  
    lexer_print_all_tokens("12a34");wait();      // Caractere inválido
lexer_print_all_tokens("1234567890123456789012345678901234567890123456789012345678901234");

    return 0;
}