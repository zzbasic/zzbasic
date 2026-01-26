// lexer_test0.4.0.c 
#include <stdio.h>
#include "color.h"
#include "lexer.h"
#include "utils.h"

int main() {
    setup_utf8();
    
    printf("%sZzBasic Lexer Test - v0.4.0 NEW TOKENS%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    // Adicione este teste ao seu test_lexer_v0.4.0.c
    printf("%sTeste de token_type_to_string():%s\n", COLOR_HEADER, COLOR_RESET);
    printf("TOKEN_PRINT = %s\n", token_type_to_string(TOKEN_PRINT));
    printf("TOKEN_QUESTION = %s\n", token_type_to_string(TOKEN_QUESTION));
    printf("TOKEN_RED = %s\n", token_type_to_string(TOKEN_RED));
    printf("TOKEN_BRED = %s\n", token_type_to_string(TOKEN_BRED));
    printf("TOKEN_WIDTH = %s\n", token_type_to_string(TOKEN_WIDTH));
    printf("TOKEN_NOCOLOR = %s\n", token_type_to_string(TOKEN_NOCOLOR));
    wait();
    
    //return 0;

    // ============================================
    // TEST 1: Comando print básico
    // ============================================
    printf("%s1. Comando print básico:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print \"Olá Mundo\"");
    wait();
    
    // ============================================
    // TEST 2: Atalho ?
    // ============================================
    printf("%s2. Atalho ?:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("? \"Teste rápido\"");
    wait();
    
    // ============================================
    // TEST 3: Print com múltiplos argumentos
    // ============================================
    printf("%s3. Print com múltiplos argumentos:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print \"Nome:\" nome \"Idade:\" idade");
    wait();
    
    // ============================================
    // TEST 4: Comandos de formatação
    // ============================================
    printf("%s4. Comandos de formatação:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print left width(10) \"ID\" right width(20) \"NOME\"");
    wait();
    
    // ============================================
    // TEST 5: Cores básicas
    // ============================================
    printf("%s5. Cores básicas:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print red \"ERRO\" nocolor green \"OK\" nocolor blue \"INFO\"");
    wait();
    
    // ============================================
    // TEST 6: Cores bright
    // ============================================
    printf("%s6. Cores bright:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print bred \"ALERTA!\" bgreen \"SUCESSO\" bblue \"NOTIFICAÇÃO\"");
    wait();
    
    // ============================================
    // TEST 7: Expressões mistas
    // ============================================
    printf("%s7. Expressões mistas:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print \"Soma:\" 10 + 20 cyan \"Resultado:\" nocolor 30");
    wait();
    
    // ============================================
    // TEST 8: Controle de linha com ;
    // ============================================
    printf("%s8. Controle de linha com ;:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print \"Linha 1\"; print \" continua\"");
    wait();
    
    // ============================================
    // TEST 9: Combinando tudo
    // ============================================
    printf("%s9. Combinando tudo:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print cyan \"=== RELATÓRIO ===\" nocolor");
    wait();
    lexer_print_all_tokens("print yellow \"ID\" nocolor width(10) \"PRODUTO\" width(20) \"QTD\"");
    wait();
    lexer_print_all_tokens("print \"101\" width(10) \"Monitor\" width(20) green \"15\" nocolor");
    wait();
    
    // ============================================
    // TEST 10: Verificação de palavras-chave vs identificadores
    // ============================================
    printf("%s10. Palavras-chave vs identificadores:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print red identifier 10");
    wait();
    
    // ============================================
    // TEST 11: Caso especial - width com número
    // ============================================
    printf("%s11. Width com número:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("print width(15) 42");
    wait();
    
    // ============================================
    // TEST 12: Identificadores que parecem cores
    // ============================================
    printf("%s12. Identificadores que parecem cores:%s\n", COLOR_HEADER, COLOR_RESET);
    lexer_print_all_tokens("let red = 10  let blue = \"azul\"  print red blue");
    wait();
    
    printf("%s✅ Todos os testes de lexer concluídos!%s\n", COLOR_SUCCESS, COLOR_RESET);

    return 0;
}