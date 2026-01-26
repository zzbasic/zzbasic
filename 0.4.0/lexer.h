// lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdarg.h>

#include "zzdefs.h"

// ============================================
// Tipos de Token (Token Types)
// ============================================
typedef enum {
    TOKEN_NULL,
    TOKEN_EOF,          // Fim de arquivo
    TOKEN_EOL,          // Fim de linha (nova linha ou ;)
    TOKEN_ERROR,        // Token de erro
    
    // Literais
    TOKEN_NUMBER,       // 3.14, -42, etc.
    TOKEN_STRING,       // literal string entre aspas
    
    // Operadores
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_PERCENT,      // %

    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )

    TOKEN_LET,          // Palavra-chave LET
    TOKEN_IDENTIFIER,   // Identificador: x, y, total, _var
    TOKEN_EQUAL,        // Operador de atribuição: =

    TOKEN_COLON,        // :
    TOKEN_SEMICOLON,    // ;

    TOKEN_NL,           // nl - controla quebra de linha no print

    TOKEN_PRINT,        // print
    TOKEN_QUESTION,     // ? (atalho para print)

    // Comandos de formatação
    TOKEN_WIDTH,        // width
    TOKEN_LEFT,         // left
    TOKEN_RIGHT,        // right
    TOKEN_CENTER,       // center
    TOKEN_NOCOLOR,      // nocolor

    // Cores normais (8)
    TOKEN_BLACK,        // black
    TOKEN_RED,          // red
    TOKEN_GREEN,        // green
    TOKEN_YELLOW,       // yellow
    TOKEN_BLUE,         // blue
    TOKEN_MAGENTA,      // magenta
    TOKEN_CYAN,         // cyan
    TOKEN_WHITE,        // white

    // Cores bright (8)
    TOKEN_BRED,         // bred
    TOKEN_BGREEN,       // bgreen
    TOKEN_BYELLOW,      // byellow
    TOKEN_BBLUE,        // bblue
    TOKEN_BMAGENTA,     // bmagenta
    TOKEN_BCYAN,        // bcyan
    TOKEN_BWHITE,       // bwhite

    // Background colors (8 - futuro)
    TOKEN_BGBLACK,      // bgblack
    TOKEN_BGRED,        // bgred
    TOKEN_BGGREEN,      // bggreen
    TOKEN_BGYELLOW,     // bgyellow
    TOKEN_BGBLUE,       // bgblue
    TOKEN_BGMAGENTA,    // bgmagenta
    TOKEN_BGCYAN,       // bgcyan
    TOKEN_BGWHITE,      // bgwhite

    TOKEN_NOERROR
    
    // Palavras-chave (reservadas para versões futuras)
    // TOKEN_LET, TOKEN_PRINT, TOKEN_IF, etc.
} TokenType;

// ============================================
// Estrutura do Token (Expandida)
// ============================================
typedef struct {
    TokenType type;            
    
    union {
        double number;              // TOKEN_NUMBER
        char string[STRING_SIZE];   // TOKEN_STRING
        char varname[VARNAME_SIZE]; // TOKEN_IDENTIFIER
    } value;
    
    char text[BUFFER_SIZE];       
    
    int line;                       // Número da linha
    int column;                     // Coluna
} Token;

// ============================================
// Estrutura do Lexer (Expandida)
// ============================================
typedef struct {
    const char* source;         // Código-fonte a ser tokenizado
    int source_size;            // Tamanho do código fonte
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

const char* token_type_to_string(TokenType type);

#endif // LEXER_H
// fim de lexer.h