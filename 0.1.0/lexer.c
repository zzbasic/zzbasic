// ============================================
// Headers da Biblioteca C
// ============================================
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ============================================
// Headers do Projeto
// ============================================
#include "color.h"
#include "utils.h"
#include "lexer.h"

// ============================================
// Constantes
// ============================================
#define MAX_NUMBER_LENGTH 63

static const char* TOKEN_STRINGS[] = 
{
    "EOF",      // TOKEN_EOF
    "EOL",      // TOKEN_EOL
    "ERROR",    // TOKEN_ERROR
    "NUMBER",   // TOKEN_NUMBER
    "PLUS",     // TOKEN_PLUS
    "MINUS",    // TOKEN_MINUS
    "STAR",     // TOKEN_STAR
    "SLASH",    // TOKEN_SLASH
    "PERCENT",  // TOKEN_PERCENT
    "LPAREN",   // TOKEN_LPAREN
    "RPAREN",   // TOKEN_RPAREN
};

// ============================================
// PROTÓTIPOS das Funções Privadas (static)
// ============================================
static void lexer_advance(Lexer* lexer);
static char lexer_peek(Lexer* lexer);
static void lexer_skip_whitespace(Lexer* lexer);

static Token lexer_make_token(Lexer* lexer,
                              Token_type type,
                              double number_value,
                              char operator_char,
                              const char* text);

static Token lexer_check_buffer_overflow(Lexer* lexer,
                                         const char* function_name,
                                         char* buffer,
                                         int current_index);

static Token lexer_report_error(Lexer* lexer,
                                const char* format,
                                ...);

static int lexer_read_digits(Lexer* lexer, char* buffer, int* index);
static Token lexer_read_number(Lexer* lexer);


//============================================
// FUNÇÕES PÚBLICAS NO FINAL
//============================================
// void lexer_init(Lexer* lexer, const char* source);
// Token lexer_get_next_token(Lexer* lexer);
// void lexer_print_token(Token token);
// void lexer_print_all_tokens(const char* source);


// ============================================
// Implementação das Funções Privadas
// ============================================
static void lexer_advance(Lexer* lexer)
{
    if (lexer->current_char != '\0')
    {
        lexer->position++;
        lexer->column++;
        
        if (lexer->current_char == '\n')
        {
            lexer->line++;
            lexer->column = 1;
        }
        
        lexer->current_char = lexer->source[lexer->position];
    }
}

static char lexer_peek(Lexer* lexer)
{
    if (lexer->source[lexer->position] == '\0')
    {
        return '\0';
    }
    return lexer->source[lexer->position + 1];
}

static void lexer_skip_whitespace(Lexer* lexer)
{
    while (lexer->current_char == ' ' || lexer->current_char == '\t')
    {
        lexer_advance(lexer);
    }
}

static Token lexer_make_token(Lexer* lexer,
                              Token_type type,
                              double number_value,
                              char operator_char,
                              const char* text)
{
    Token token;
    
    // Campos obrigatórios
    token.type = type;
    token.type_string = TOKEN_STRINGS[type];
    token.line = lexer->line;
    token.column = lexer->column;
    
    // Valores (dependem do tipo)
    token.value.number_value = number_value;
    token.value.operator_char = operator_char;
    
    // Texto (se fornecido)
    if (text != NULL)
    {
        strncpy(token.text, text, TOKEN_TEXT_SIZE - 1);
        token.text[TOKEN_TEXT_SIZE - 1] = '\0';
    }
    else
    {
        token.text[0] = '\0';
    }
    
    return token;
}

// Função auxiliar para verificar buffer overflow e reportar erro
static Token lexer_check_buffer_overflow(Lexer* lexer,
                                         const char* function_name,
                                         char* buffer,
                                         int current_index)
{
    if (current_index >= MAX_NUMBER_LENGTH)
    {
        buffer[MAX_NUMBER_LENGTH] = '\0';
        return lexer_report_error(lexer,
                                  function_name,
                                  "buffer overflow (maximum %d characters)",
                                  MAX_NUMBER_LENGTH);      
    }
    
    return lexer_make_token(lexer,
                            TOKEN_NOERROR,
                            0,
                            ' ',
                            "");
}

static Token lexer_report_error(Lexer* lexer,
                                const char* format,
                                ...)
{
    // Formata a mensagem
    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Formata a mensagem final com cores
    char error_text[BUFFER_SIZE];
    snprintf(error_text,
             sizeof(error_text),
             "Error: %s",
             message);
    
    return lexer_make_token(lexer,
                            TOKEN_ERROR,
                            0.0,
                            ' ',
                            error_text);
}

static int lexer_read_digits(Lexer* lexer,
                             char* buffer,
                             int* index)
{
    while (isdigit(lexer->current_char) && lexer->current_char != '\0')
    {
        Token error_check = lexer_check_buffer_overflow(lexer, __func__, buffer, *index);
        if (error_check.type == TOKEN_ERROR)
        {
            return 0;
        }

        buffer[(*index)++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    return 1;
}

static Token lexer_read_number(Lexer* lexer)
{
    char buffer[MAX_NUMBER_LENGTH + 1];
    int i = 0;

    // Parte inteira
    if (!lexer_read_digits(lexer, buffer, &i))
    {
        // Se houver erro, lexer_check_buffer_overflow já reportou
        // Precisamos retornar o erro
        // Mas como? Veja a solução 2 para uma abordagem melhor
        return lexer_report_error(lexer, 
                                 "buffer overflow while reading integer part");
    }

    // Parte decimal (opcional)
    if (lexer->current_char == '.')
    {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);

        if (!isdigit(lexer->current_char))
        {
            // Adiciona o char inválido ao buffer para mostrar no erro
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);

            // Tenta ler mais dígitos (para capturar o padrão inválido completo)
            lexer_read_digits(lexer, buffer, &i);  // Ignora erro aqui, só quer capturar
            
            buffer[i] = '\0';

            return lexer_report_error(lexer,"invalid number '%s'", buffer);     
        }

        // Lê os dígitos após o ponto decimal
        if (!lexer_read_digits(lexer, buffer, &i))
        {
            return lexer_report_error(lexer, "buffer overflow while reading decimal part");
        }
    }
    // Fim da parte decimal

    // DETECTA SEGUNDO PONTO (BUG!)
    if (lexer->current_char == '.')
    {
        Token error_check = lexer_check_buffer_overflow(lexer, __func__, buffer, i);
        if (error_check.type == TOKEN_ERROR) return error_check;

        // Adiciona o segundo ponto ao buffer para mostrar no erro
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);

        // Tenta ler mais dígitos (para capturar o padrão inválido completo)
        lexer_read_digits(lexer, buffer, &i);  // Ignora erro aqui, só quer capturar

        buffer[i] = '\0';
        return lexer_report_error(lexer, "invalid number '%s'", buffer);           
    } 

    // Termina string
    buffer[i] = '\0';

    // Converte para double
    char* endptr;
    double valor = strtod(buffer, &endptr);
    
    if (*endptr != '\0')
    {
        return lexer_report_error(lexer, "internal error converting number '%s'",
                                  buffer);
    }

    return lexer_make_token(lexer,
                            TOKEN_NUMBER,
                            valor,
                            ' ',
                            buffer);
}


// ============================================
// Implementação das Funções Públicas
// ============================================
void lexer_init(Lexer* lexer,
                const char* source)
{
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = source[0];
}

Token lexer_get_next_token(Lexer* lexer)
{
    // Pular espaços em branco
    lexer_skip_whitespace(lexer);
    
    char c = lexer->current_char;
    
    // Fim de arquivo
    if (c == '\0')
    {
        return lexer_make_token(lexer,
                                TOKEN_EOF,
                                0.0,
                                ' ',
                                NULL);
    }
    
    // Números
    if (isdigit(c) || c == '.')
    {
        return lexer_read_number(lexer);
    }
    
    // Operadores e caracteres especiais
    switch (c)
    {
        case '+':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_PLUS,
                                    0.0,
                                    '+',
                                    "+");
            
        case '-':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_MINUS,
                                    0.0,
                                    '-',
                                    "-");
            
        case '*':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_STAR,
                                    0.0,
                                    '*',
                                    "*");
            
        case '/':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_SLASH,
                                    0.0,
                                    '/',
                                    "/");
            
        case '%':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_PERCENT,
                                    0.0,
                                    '%',
                                    "%");
            
        case '(':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_LPAREN,
                                    0.0,
                                    '(',
                                    "(");
            
        case ')':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_RPAREN,
                                    0.0,
                                    ')',
                                    ")");
            
        case '\n':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_EOL,
                                    0.0,
                                    ' ',
                                    "\\n");
            
        default:
        {
            char error_msg[BUFFER_SIZE];
            snprintf(error_msg,
                     sizeof(error_msg),
                     "Unexpected character: '%c' (ASCII %d)",
                     c,
                     (int)c);
            return lexer_report_error(lexer, "%s", error_msg);
        }
    }
}

void lexer_print_token(Token token)
{
    printf("[%s]", token.type_string);
    printf("(%d:%d)", token.line, token.column);
    
    if (token.type == TOKEN_NUMBER)
    {
        printf(": %g", token.value.number_value);
        if (token.text[0] != '\0')
        {
            printf(" (texto: %s)", token.text);
        }
    }
    else if (token.type == TOKEN_ERROR)
    {
        printf("%s\n%s\n%s", COLOR_ERROR, token.text, COLOR_RESET);
    }
    else if (token.text[0] != '\0')
    {
        printf(": %s", token.text);
    }
}

void lexer_print_all_tokens(const char* source)
{
    printf("=== ANÁLISE LÉXICA ===\n");
    printf("Código-fonte: \"%s\"\n\n", source);
    
    Lexer lexer;
    lexer_init(&lexer, source);
    
    int token_count = 0;
    Token token;
    
    do
    {
        token = lexer_get_next_token(&lexer);
        printf("%3d: ", ++token_count);
        lexer_print_token(token);
        printf("\n");
    }
    while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
    
    if (token.type == TOKEN_ERROR)
    {
        printf(COLOR_WARNING "\nAnálise interrompida devido a erro léxico.\n" COLOR_RESET);
    }
    
    printf("\nTotal de tokens: %d\n", token_count);
    printf("=== FIM DA ANÁLISE ===\n");
}
// fim de lexer.c