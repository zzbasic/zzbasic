// test_final_zz.c
#include <stdio.h>
#include "lexer.h"

const char* get_token_name(Token_type type) {
    // Retorna o nome baseado no seu enum atual
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_EOL: return "EOL";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_PERCENT: return "PERCENT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_ID: return "ID";
        case TOKEN_LET: return "LET";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NOERROR: return "NOERROR";
        default: return "UNKNOWN";
    }
}

int main() {
    printf("=== TESTE FINAL ZzBasic v0.2.0 ===\n\n");
    
    // Teste 1: Verificar tokenização básica
    const char* test_code = "let x = 10 + y # comentário\n(x * 2)";
    
    Lexer lexer;
    lexer_init(&lexer, test_code);
    
    Token token;
    int count = 0;
    
    printf("Código: \"%s\"\n", test_code);
    printf("\nTokens gerados:\n");
    
    do {
        token = lexer_get_next_token(&lexer);
        printf("%2d. ", ++count);
        
        // Verificar se type_string está correto
        const char* expected_name = get_token_name(token.type);
        
        if (token.type_string && strcmp(token.type_string, expected_name) != 0) {
            printf("INCONSISTÊNCIA: type=%d, type_string='%s', esperado='%s'\n",
                   token.type, token.type_string, expected_name);
        } else {
            printf("[%s](%d:%d)", 
                   token.type_string ? token.type_string : "NULL",
                   token.line, token.column);
            
            // Mostrar valor/texto
            if (token.type == TOKEN_NUMBER) {
                printf(" = %g", token.value.number_value);
                if (token.text[0]) printf(" (texto: %s)", token.text);
            }
            else if (token.text[0]) {
                printf(" '%s'", token.text);
            }
            printf("\n");
        }
        
    } while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
    
    // Teste 2: Verificar enum vs TOKEN_STRINGS
    printf("\n=== VERIFICAÇÃO ENUM vs STRINGS ===\n");
    
    int enum_count = TOKEN_NOERROR + 1;  // Último valor + 1
    int strings_count = sizeof(TOKEN_STRINGS) / sizeof(TOKEN_STRINGS[0]);
    
    printf("Tokens no enum: %d\n", enum_count);
    printf("Strings em TOKEN_STRINGS: %d\n", strings_count);
    
    if (enum_count != strings_count) {
        printf("ERRO: Número diferente! Enum=%d, Strings=%d\n", 
               enum_count, strings_count);
        
        // Mostrar todas as strings
        printf("\nTOKEN_STRINGS[] = {\n");
        for (int i = 0; i < strings_count; i++) {
            printf("  [%2d] = \"%s\"\n", i, TOKEN_STRINGS[i]);
        }
        printf("}\n");
    } else {
        printf("OK: Enum e strings têm a mesma quantidade.\n");
    }
    
    // Teste 3: Teste rápido de casos específicos
    printf("\n=== TESTES RÁPIDOS ===\n");
    
    struct {
        const char* input;
        const char* desc;
    } testes[] = {
        {"let", "palavra-chave 'let'"},
        {"Let", "identificador 'Let' (case-sensitive)"},
        {"x = 5", "atribuição simples"},
        {"var_123", "identificador com números"},
        {"# só comentário\n", "comentário com nova linha"},
        {"12.34", "número decimal"},
        {"12.34.56", "número inválido"},
    };
    
    for (int i = 0; i < sizeof(testes)/sizeof(testes[0]); i++) {
        printf("\nTeste %d: %s\n", i+1, testes[i].desc);
        printf("  Entrada: \"%s\"\n", testes[i].input);
        
        Lexer l;
        lexer_init(&l, testes[i].input);
        
        Token t = lexer_get_next_token(&l);
        printf("  Token: [%s](%d:%d)", 
               t.type_string, t.line, t.column);
        if (t.text[0]) printf(" '%s'", t.text);
        if (t.type == TOKEN_NUMBER) printf(" = %g", t.value.number_value);
        printf("\n");
        
        if (t.type == TOKEN_ERROR) {
            printf("  ERRO: %s\n", t.text);
        }
    }
    
    return 0;
}