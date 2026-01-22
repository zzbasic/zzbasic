// ============================================
// Headers da Biblioteca C
// ============================================
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "color.h"
#include "lexer.h"

static const char* TOKEN_STRINGS[] = 
{
    "EOF",          // TOKEN_EOF
    "EOL",          // TOKEN_EOL
    "ERROR",        // TOKEN_ERROR
    "NUMBER",       // TOKEN_NUMBER
    "PLUS",         // TOKEN_PLUS
    "MINUS",        // TOKEN_MINUS
    "STAR",         // TOKEN_STAR
    "SLASH",        // TOKEN_SLASH
    "PERCENT",      // TOKEN_PERCENT
    "LPAREN",       // TOKEN_LPAREN
    "RPAREN",       // TOKEN_RPAREN
    "IDENTIFIER",   // TOKEN_IDENTIFIER           
    "LET",          // TOKEN_LET          
    "EQUAL",        // TOKEN_EQUAL 
    "NOERROR"       // TOKEN_NOERROR
};

// ============================================
// PROTÓTIPOS das Funções Privadas (static)
// ============================================
static void lexer_advance(Lexer* lexer);
static char lexer_peek(Lexer* lexer);
static char lexer_peek_next(Lexer* lexer);
static void lexer_skip_whitespace(Lexer* lexer);

static Token lexer_make_token(Lexer* lexer,
                              Token_type type,
                              double number_value,
                              char operator_char,
                              const char* text,
                              int line,
                              int column);

static Token lexer_check_buffer_overflow(Lexer* lexer,
                                         char* buffer,
                                         int current_index);

static Token lexer_report_error(Lexer* lexer,
                                int line,
                                int column,
                                const char* format,
                                ...);

static int lexer_read_digits(Lexer* lexer, char* buffer, int* index);
static Token lexer_read_number(Lexer* lexer);
static Token lexer_read_identifier(Lexer* lexer);


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

/*******************************************************************
Em UTF-8 as letras acentuadas ocupam 2 bytes, então a contagem
não pode ser caractere por caractere.

Regra geral:
Qualquer caractere latino acentuado (maiúsculo ou minúsculo)
ocupa 2 bytes em UTF-8, exceto:
- Letras do alfabeto básico (A-Z, a-z): 1 byte
- Números (0-9): 1 byte
- Pontuação básica (. , ; : ! ? etc.): 1 byte

Exemplo:
'é' - 2 bytes (0xC3 0xA9); 0xC3 = 11000011₂; 0xA9 = 10101001₂
'ã' - 2 bytes (0xC3 0xA3); 0xC3 = 11000011₂; 0xA3 = 10100011₂

Note que se contarmos caracteres teremos contado dois,
mas na verdade queremos contar apenas um.

Note também que o segundo byte começa com 10.

Bytes começando com 0xxxxxxx (0x00-0x7F): ASCII, 1 byte

Bytes começando com 110xxxxx: início de caractere 2-byte

Bytes começando com 10xxxxxx: byte de continuação

Seu código (lexer->current_char & 0b11000000) != 0b10000000 verifica:

Se NÃO é um byte de continuação (10xxxxxx)

Então é um novo caractere ==> incrementa coluna
*******************************************************************/
static void lexer_advance(Lexer* lexer)
{
    // Converte para unsigned para evitar problemas com signed char
    unsigned char uc = (unsigned char)lexer->current_char;

    if(lexer->current_char == '\n')
    {        
        lexer->line++;
        lexer->column = 1;
    }
    else
    {
        if ((uc & 0b11000000) != 0b10000000) {
            lexer->column++;        
        }
    }

    lexer->position++;
    if (lexer->position >= lexer->source_size)
    {
        lexer->current_char = '\0';
    }
    else
    {
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

// Retorna o próximo caractere sem avançar (lookahead)
static char lexer_peek_next(Lexer* lexer)
{
    if (lexer->position + 1 < lexer->source_size)
    {
        return lexer->source[lexer->position + 1];
    }
    return '\0';
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
                              const char* text,
                              int line,
                              int column)
{
    Token token;
    
    // Campos obrigatórios
    token.type = type;
    token.type_string = TOKEN_STRINGS[type];
    token.line = line;
    token.column = column;
    
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
                                         char* buffer,
                                         int current_index)
{
    if (current_index >= MAX_NUMBER_LENGTH)
    {
        buffer[MAX_NUMBER_LENGTH] = '\0';
        return lexer_report_error(lexer,
                                  lexer->line,
                                  lexer->column,
                                  "buffer overflow (maximum %d characters)",
                                  MAX_NUMBER_LENGTH);      
    }
    
    return lexer_make_token(lexer,
                            TOKEN_NOERROR,
                            0,
                            ' ',
                            "",
                            0,
                            0);
}

static Token lexer_report_error(Lexer* lexer,
                                int line,
                                int column,
                                const char* format,
                                ...)
{
    // Formata a mensagem
    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    char error_text[BUFFER_SIZE];
    snprintf(error_text,
             sizeof(error_text),
             "Error: %s",
             message);
    
    return lexer_make_token(lexer,
                            TOKEN_ERROR,
                            0.0,
                            ' ',
                            error_text,
                            line,
                            column);
}

static int lexer_read_digits(Lexer* lexer,
                             char* buffer,
                             int* index)
{
    while (isdigit(lexer->current_char) && lexer->current_char != '\0')
    {
        Token error_check = lexer_check_buffer_overflow(lexer, buffer, *index);
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

    int nr_line = lexer->line;
    int nr_column = lexer->column;

    // Parte inteira
    if (!lexer_read_digits(lexer, buffer, &i))
    {
        return lexer_report_error(lexer,
                                  nr_line,
                                  nr_column, 
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

            return lexer_report_error(lexer, nr_line, nr_column, "invalid number '%s'", buffer);     
        }

        // Lê os dígitos após o ponto decimal
        if (!lexer_read_digits(lexer, buffer, &i))
        {
            return lexer_report_error(lexer,
                                      nr_line,
                                      nr_column,
                                      "buffer overflow while reading decimal part");
        }
    }
    // Fim da parte decimal

    // DETECTA SEGUNDO PONTO (BUG!)
    if (lexer->current_char == '.')
    {
        Token error_check = lexer_check_buffer_overflow(lexer, buffer, i);
        if (error_check.type == TOKEN_ERROR) return error_check;

        // Adiciona o segundo ponto ao buffer para mostrar no erro
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);

        // Tenta ler mais dígitos (para capturar o padrão inválido completo)
        lexer_read_digits(lexer, buffer, &i);  // Ignora erro aqui, só quer capturar

        buffer[i] = '\0';
        return lexer_report_error(lexer, nr_line, nr_column, "invalid number '%s'", buffer);           
    } 

    // Termina string
    buffer[i] = '\0';

    // Converte para double
    char* endptr;
    double valor = strtod(buffer, &endptr);
    
    if (*endptr != '\0')
    {
        return lexer_report_error(lexer, nr_line, nr_column, "internal error converting number '%s'",
                                  buffer);
    }

    return lexer_make_token(lexer,
                            TOKEN_NUMBER,
                            valor,
                            ' ',
                            buffer,
                            nr_line,
                            nr_column);
}

static Token lexer_read_identifier(Lexer* lexer)
{
    char buffer[MAX_IDENTIFIER_LEN + 1];
    int i = 0;

    int id_line = lexer->line;
    int id_column = lexer->column;
    
    // Lê o identificador
    while (isalnum(lexer->current_char) || lexer->current_char == '_')
    {
        if (i >= MAX_IDENTIFIER_LEN)
        {
            buffer[MAX_IDENTIFIER_LEN] = '\0';
            return lexer_report_error(lexer,
                                      id_line,
                                      id_column,
                                      "Identifier too long (max %d chars): '%.30s...'",
                                      MAX_IDENTIFIER_LEN,
                                      buffer);
        }
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    buffer[i] = '\0';
    
    // Verifica palavras-chave 
    if (strcmp(buffer, "let") == 0)
    {
        return lexer_make_token(lexer, TOKEN_LET, 0.0, ' ', "let", id_line, id_column);
    }
    // Futuras palavras-chave:
    // else if (strcmp(buffer, "print") == 0) return TOKEN_PRINT;
    // else if (strcmp(buffer, "if") == 0) return TOKEN_IF;
    // else if (strcmp(buffer, "then") == 0) return TOKEN_THEN;
    
    // É identificador normal (preserva o case do usuário)
    return lexer_make_token(lexer, TOKEN_IDENTIFIER, 0.0, ' ', buffer, id_line, id_column);
}



// ============================================
// Implementação das Funções Públicas
// ============================================
void lexer_init(Lexer* lexer,
                const char* source)
{
    lexer->source = source;
    lexer->source_size = (int)strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = source[0];
}

Token lexer_get_next_token(Lexer* lexer)
{
    while (1)  
    {
        lexer_skip_whitespace(lexer);

        // COMENTÁRIOS
        if (lexer->current_char == '#')
        {
            // Pula tudo até fim da linha MAS NÃO consome o \n
            while (lexer->current_char != '\n' && 
                   lexer->current_char != '\0')
            {
                lexer_advance(lexer);
            }
            // NÃO consome o \n! Ele será retornado como TOKEN_EOL
            // Continue no loop para verificar se há mais espaços/comentários
            continue;
        }
        // Se não é comentário, sai do loop e processa o token
        break;
    }

    // AGORA processa o token normalmente
    int line = lexer->line;
    int column = lexer->column;        
    char c = lexer->current_char;
    
    // Fim de arquivo
    if (c == '\0')
    {
        return lexer_make_token(lexer,
                                TOKEN_EOF,
                                0.0,
                                ' ',
                                NULL,
                                line,
                                column);
    }
    
    // Números
    if (isdigit(c) || c == '.')
    {
        return lexer_read_number(lexer);
    }
    
    // Identificadores
    if (isalpha(c) || c == '_')
    {
        return lexer_read_identifier(lexer);
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
                                    "+",
                                    line,
                                    column);
            
        case '-':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_MINUS,
                                    0.0,
                                    '-',
                                    "-",
                                    line,
                                    column);
            
        case '*':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_STAR,
                                    0.0,
                                    '*',
                                    "*",
                                    line,
                                    column);
            
        case '/':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_SLASH,
                                    0.0,
                                    '/',
                                    "/",
                                    line,
                                    column);
            
        case '(':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_LPAREN,
                                    0.0,
                                    '(',
                                    "(",
                                    line,
                                    column);
            
        case ')':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_RPAREN,
                                    0.0,
                                    ')',
                                    ")",
                                    line,
                                    column);
        case '=':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_EQUAL,
                                    0.0,
                                    '=',
                                    "=",
                                    line,
                                    column);            
        case '\n':
            lexer_advance(lexer);
            return lexer_make_token(lexer,
                                    TOKEN_EOL,
                                    0.0,
                                    ' ',
                                    "\\n",
                                    line,
                                    column);
            
        default:
        {
            char error_msg[BUFFER_SIZE];
            snprintf(error_msg,
                     sizeof(error_msg),
                     "Unexpected character: '%c' (ASCII %d)",
                     c,
                     (int)c);
            return lexer_report_error(lexer, line, column, "%s", error_msg);
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
    else if (token.type == TOKEN_IDENTIFIER)
    {
        printf(": %s", token.text);
    }
    else if (token.type == TOKEN_LET)
    {
        printf(": %s", token.text);
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