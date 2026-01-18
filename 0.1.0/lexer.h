#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdarg.h>

#include "utils.h"

#define MAX_NUMBER_LEN 63 

// ============================================
// Tipos de Token (Token Types)
// ============================================
typedef enum {
    // Fim de arquivo/linha
    TOKEN_EOF,
    TOKEN_EOL,          // Fim de linha (nova linha ou ;)
    TOKEN_ERROR,        // NOVO: Token de erro
    
    // Literais
    TOKEN_NUMBER,       // 3.14, -42, etc.
    
    // Operadores
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_PERCENT,      // %
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    
    // Palavras-chave (reservadas para versões futuras)
    // TOKEN_LET, TOKEN_PRINT, TOKEN_IF, etc.
} TokenType;

// ============================================
// Estrutura do Token (Expandida)
// ============================================
typedef struct {
    TokenType type;             // Tipo enumerado
    const char* type_string;    // String do tipo (para debug)
    
    // Valor do token (depende do tipo)
    union {
        double number_value;    // Para TOKEN_NUMBER
        char operator_char;     // Para operadores (+, -, etc.)
    } value;
    
    // Texto original do token (útil para erros, debugging)
    char text[TOKEN_TEXT_SIZE];              // Texto original do token
    
    int line;                   // Número da linha
    int column;                 // Coluna
} Token;

// ============================================
// Estrutura do Lexer (Expandida)
// ============================================
typedef struct {
    const char* source;         // Código-fonte a ser tokenizado
    int position;               // Posição atual no source
    int line;                   // Linha atual
    int column;                 // Coluna atual
    char current_char;          // NOVO: caractere atual (para conveniência)
} Lexer;

// ============================================
// Interface Pública do Lexer
// ============================================

// Inicialização e operação
void lexer_init(Lexer* lexer, const char* source);
Token lexer_get_next_token(Lexer* lexer);

// Funções de utilidade/debug
void lexer_print_token(Token token);
void lexer_print_all_tokens(const char* source);

#endif // LEXER_H