// lexer.c

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> 

#include "color.h"
#include "lexer.h"

static void lexer_advance(Lexer* lexer);
static char lexer_peek(Lexer* lexer);
static char lexer_peek_next(Lexer* lexer);
static void lexer_skip_whitespace(Lexer* lexer);

static Token lexer_make_token(Lexer* lexer,
                              TokenType type,
                              double number_value,
                              char operator_char,
                              const char* text,
                              int line,
                              int column);

static Token lexer_check_buffer_overflow_number(Lexer* lexer,
                                                char* buffer,
                                                int current_index);

static Token lexer_report_error(Lexer* lexer,
                                int line,
                                int column,
                                const char* format,
                                ...);

static int lexer_read_digits(Lexer* lexer, char* buffer, int* index);
static Token lexer_read_number(Lexer* lexer);
static TokenType check_keyword(const char *lexeme);
static Token lexer_read_identifier(Lexer* lexer);
static Token lexer_read_string(Lexer* lexer);


static const char* TOKEN_STRINGS[] = 
{
    "NULL",             // TOKEN_NULL      
    "EOF",              // TOKEN_EOF
    "EOL",              // TOKEN_EOL
    "ERROR",            // TOKEN_ERROR

    "NUMBER",           // TOKEN_NUMBER
    "STRING",           // TOKEN_STRING

    "PLUS",             // TOKEN_PLUS
    "MINUS",            // TOKEN_MINUS
    "STAR",             // TOKEN_STAR
    "SLASH",            // TOKEN_SLASH
    "PERCENT",          // TOKEN_PERCENT

    "LPAREN",           // TOKEN_LPAREN
    "RPAREN",           // TOKEN_RPAREN

    "LET",              // TOKEN_LET          
    "IDENTIFIER",       // TOKEN_IDENTIFIER 
    "ASSIGN",           // TOKEN_ASSIGN 

    "COLON",            // TOKEN_COLON      
    "SEMICOLON",        // TOKEN_SEMICOLON   

    "NL",               // TOKEN_NL   

    "PRINT",            // TOKEN_PRINT       
    "QUESTION",         // TOKEN_QUESTION     

    // Comandos de formatação
    "WIDTH",            // TOKEN_WIDTH
    "LEFT",             // TOKEN_LEFT
    "RIGHT",            // TOKEN_RIGHT
    "CENTER",           // TOKEN_CENTER
    "NOCOLOR",          // TOKEN_NOCOLOR

    // Cores normais (8)
    "BLACK",            // TOKEN_BLACK
    "RED",              // TOKEN_RED
    "GREEN",            // TOKEN_GREEN
    "YELLOW",           // TOKEN_YELLOW
    "BLUE",             // TOKEN_BLUE
    "MAGENTA",          // TOKEN_MAGENTA
    "CYAN",             // TOKEN_CYAN
    "WHITE",            // TOKEN_WHITE

    // Cores bright (8)
    "BRED",             // TOKEN_BRED
    "BGREEN",           // TOKEN_BGREEN
    "BYELLOW",          // TOKEN_BYELLOW
    "BBLUE",            // TOKEN_BBLUE
    "BMAGENTA",         // TOKEN_BMAGENTA
    "BCYAN",            // TOKEN_BCYAN
    "BWHITE",           // TOKEN_BWHITE

    // Background colors (8 - futuro)
    "BGBLACK",          // TOKEN_BGBLACK
    "BGRED",            // TOKEN_BGRED
    "BGGREEN",          // TOKEN_BGGREEN
    "BGYELLOW",         // TOKEN_BGYELLOW
    "BGBLUE",           // TOKEN_BGBLUE
    "BGMAGENTA",        // TOKEN_BGMAGENTA
    "BGCYAN",           // TOKEN_BGCYAN
    "BGWHITE",          // TOKEN_BGWHITE

    "INPUT",            // TOKEN_INPUT
    "TRUE",             // TOKEN_TRUE
    "FALSE",            // TOKEN_FALSE

    "IF",               // TOKEN_IF
    "THEN",             // TOKEN_THEN
    "ELSE",             // TOKEN_ELSE
    "END",              // TOKEN_END
    
    "AND",              // TOKEN_AND
    "OR",               // TOKEN_OR
    "NOT",              // TOKEN_NOT
    
    "EQUAL",            // TOKEN_EQUAL
    "NOT EQUAL",        // TOKEN_NOT_EQUAL
    "LESS",             // TOKEN_LESS
    "GREATER",          // TOKEN_GREATER
    "LESS_EQUAL",       // TOKEN_LESS_EQUAL 
    "GREATER EQUAL",    // TOKEN_GREATER_EQUAL

    "NOERROR"           // TOKEN_NOERROR
};


// ==================================================================
// Tabela de palavras-chave para busca rápida
// ==================================================================
typedef struct
{
    const char *lexeme;
    TokenType type;
} Keyword;

static Keyword keywords[] = 
{
    {"let", TOKEN_LET},
    {"nl", TOKEN_NL},
    {"print", TOKEN_PRINT},
    {"width", TOKEN_WIDTH},
    {"left", TOKEN_LEFT},
    {"right", TOKEN_RIGHT},
    {"center", TOKEN_CENTER},
    {"nocolor", TOKEN_NOCOLOR},
    {"black", TOKEN_BLACK},
    {"red", TOKEN_RED},
    {"green", TOKEN_GREEN},
    {"yellow", TOKEN_YELLOW},
    {"blue", TOKEN_BLUE},
    {"magenta", TOKEN_MAGENTA},
    {"cyan", TOKEN_CYAN},
    {"white", TOKEN_WHITE},
    {"bred", TOKEN_BRED},
    {"bgreen", TOKEN_BGREEN},
    {"byellow", TOKEN_BYELLOW},
    {"bblue", TOKEN_BBLUE},
    {"bmagenta", TOKEN_BMAGENTA},
    {"bcyan", TOKEN_BCYAN},
    {"bwhite", TOKEN_BWHITE},
    {"bgblack", TOKEN_BGBLACK},
    {"bgred", TOKEN_BGRED},
    {"bggreen", TOKEN_BGGREEN},
    {"bgyellow", TOKEN_BGYELLOW},
    {"bgblue", TOKEN_BGBLUE},
    {"bgmagenta", TOKEN_BGMAGENTA},
    {"bgcyan", TOKEN_BGCYAN},
    {"bgwhite", TOKEN_BGWHITE},
    {"input", TOKEN_INPUT},
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},

    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"else", TOKEN_ELSE},
    {"end", TOKEN_END},

    {"and", TOKEN_AND},
    {"or", TOKEN_OR},
    {"not", TOKEN_NOT},

    {NULL, TOKEN_NULL}
};

const char* token_type_to_string(TokenType type)
{
    if (type >= 0 && type < (sizeof(TOKEN_STRINGS) / sizeof(TOKEN_STRINGS[0])))
    {
        return TOKEN_STRINGS[type];
    }
    return "UNKNOWN";
}


// ============================================
// Implementation of Private Functions
// ============================================

/*******************************************************************
In UTF-8 accented letters occupy 2 bytes, so counting
cannot be character by character.

General rule:
Any accented Latin character (uppercase or lowercase)
occupies 2 bytes in UTF-8, except:
- Basic alphabet letters (A-Z, a-z): 1 byte
- Numbers (0-9): 1 byte
- Basic punctuation (. , ; : ! ? etc.): 1 byte

Example:
'é' - 2 bytes (0xC3 0xA9); 0xC3 = 11000011₂; 0xA9 = 10101001₂
'ã' - 2 bytes (0xC3 0xA3); 0xC3 = 11000011₂; 0xA3 = 10100011₂

Note that if we count characters we'll have counted two,
but we actually want to count only one.

Also note that the second byte starts with 10.

Bytes starting with 0xxxxxxx (0x00-0x7F): ASCII, 1 byte

Bytes starting with 110xxxxx: start of 2-byte character

Bytes starting with 10xxxxxx: continuation byte

Your code (lexer->current_char & 0b11000000) != 0b10000000 checks:

If it's NOT a continuation byte (10xxxxxx)

Then it's a new character ==> increment column
*******************************************************************/
static void lexer_advance(Lexer* lexer)
{
    // Convert to unsigned to avoid problems with signed char
    unsigned char uc = (unsigned char)lexer->current_char;

    if(lexer->current_char == '\n')
    {        
        lexer->line++;
        lexer->column = 1;
    }
    else
    {
        if ((uc & 0b11000000) != 0b10000000)
        {
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

// Returns the next character without advancing (lookahead)
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
    while (lexer->current_char == ' ' || 
           lexer->current_char == '\t' ||
           lexer->current_char == '\r')
    {  
        lexer_advance(lexer);
    }
}


// Helper function to check buffer overflow and report error
static Token lexer_check_buffer_overflow_number(Lexer* lexer,
                                                char* buffer,
                                                int current_index)
{
    if (current_index > NUMBER_SIZE - 1)
    {
        buffer[NUMBER_SIZE - 1] = '\0';
        return lexer_report_error(lexer,
                                  lexer->line,
                                  lexer->column,
                                  "buffer overflow (maximum %d characters)",
                                  NUMBER_SIZE);      
    }
 
    Token token;
    memset(&token, 0, sizeof(token));  
    token.type = TOKEN_NOERROR;
    return token;
}

static Token lexer_report_error(Lexer* lexer,
                                int line,
                                int column,
                                const char* format,
                                ...)
{
    // Format the message
    char message[TOKENTEXT_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    char error_text[TOKENTEXT_SIZE];
    snprintf(error_text,
             sizeof(error_text),
             "Error: %s",
             message);
    
    Token token;
    memset(&token, 0, sizeof(token));  

    token.type = TOKEN_ERROR;
    strncpy(token.text, error_text, TOKENTEXT_SIZE - 1);
    token.text[TOKENTEXT_SIZE - 1] = '\0';
    token.line = line;
    token.column = column;

    return token;
}

static int lexer_read_digits(Lexer* lexer,
                             char* buffer,
                             int* index)
{
    while (isdigit(lexer->current_char) && lexer->current_char != '\0')
    {
        // CHECK BUFFER OVERFLOW
        if((*index) > NUMBER_SIZE - 1)
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
    char buffer[NUMBER_SIZE];
    int i = 0;

    int nr_line = lexer->line;
    int nr_column = lexer->column;

    // Integer part
    if (!lexer_read_digits(lexer, buffer, &i))
    {
        return lexer_report_error(lexer,
                                  nr_line,
                                  nr_column, 
                                 "buffer overflow while reading integer part");
    }

    // Decimal part (optional)
    if (lexer->current_char == '.')
    {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);

        if (!isdigit(lexer->current_char))
        {
            // Add invalid char to buffer to show in error
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);

            // Try to read more digits (to capture complete invalid pattern)
            lexer_read_digits(lexer, buffer, &i);  // Ignore error here, just want to capture
            
            buffer[i] = '\0';

            return lexer_report_error(lexer, nr_line, nr_column, "invalid number '%s'", buffer);     
        }

        // Read digits after decimal point
        if (!lexer_read_digits(lexer, buffer, &i))
        {
            return lexer_report_error(lexer,
                                      nr_line,
                                      nr_column,
                                      "buffer overflow while reading decimal part");
        }
    }
    // End of decimal part

    // DETECT SECOND POINT (BUG!)
    if (lexer->current_char == '.')
    {
        Token error_check = lexer_check_buffer_overflow_number(lexer, buffer, i);
        if (error_check.type == TOKEN_ERROR) return error_check;

        // Add second point to buffer to show in error
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);

        // Try to read more digits (to capture complete invalid pattern)
        lexer_read_digits(lexer, buffer, &i);  // Ignore error here, just want to capture

        buffer[i] = '\0';
        return lexer_report_error(lexer, nr_line, nr_column, "invalid number '%s'", buffer);           
    } 

    // Terminate string
    buffer[i] = '\0';

    // Convert to double
    char* endptr;
    double valor = strtod(buffer, &endptr);
    
    if (*endptr != '\0')
    {
        return lexer_report_error(lexer, nr_line, nr_column, "internal error converting number '%s'",
                                  buffer);
    }

    Token token;
    memset(&token, 0, sizeof(token));

    token.type = TOKEN_NUMBER;
    token.value.number = valor;
    strncpy(token.text, buffer, TOKENTEXT_SIZE - 1);
    token.text[TOKENTEXT_SIZE - 1] = '\0';
    token.line = nr_line;
    token.column = nr_column;

    return token;
}

// Verifica se o lexeme é uma palavra-chave
static TokenType check_keyword(const char *lexeme)
{
    for (int i = 0; keywords[i].lexeme != NULL; i++)
    {
        if (strcmp(lexeme, keywords[i].lexeme) == 0)
        {
            return keywords[i].type;
        }
    }
    return TOKEN_NULL; // Não é palavra-chave
}

static Token lexer_read_identifier(Lexer* lexer)
{
    char buffer[VARNAME_SIZE];
    int i = 0;

    int id_line = lexer->line;
    int id_column = lexer->column;
    
    // Read identifier
    while (isalnum(lexer->current_char) || lexer->current_char == '_')
    {
        if (i > VARNAME_SIZE - 1)
        {
            buffer[VARNAME_SIZE - 1] = '\0';
            return lexer_report_error(lexer,
                                      id_line,
                                      id_column,
                                      "Identifier too long (max %d chars): '%.30s...'",
                                      VARNAME_SIZE,
                                      buffer);
        }
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    buffer[i] = '\0';
    
    Token token;
    memset(&token, 0, sizeof(token));

    // Check keywords
    TokenType token_type = check_keyword(buffer);
    if (token_type != TOKEN_NULL)
    {
        token.type = token_type;
        strcpy(token.text, buffer);
        strcpy(token.value.string, buffer);
    }
    //================ SE CHEGOU AQUI NÃO É PALAVRA-CHAVE ==========================
    else
    {
        token.type = TOKEN_IDENTIFIER;
        strcpy(token.value.varname, buffer);
    }

    token.line = id_line;
    token.column = id_column;

    return token; 
}

static Token lexer_read_string(Lexer* lexer) {
    char buffer[STRING_SIZE];
    int i = 0;

    int str_line = lexer->line;
    int str_column = lexer->column;
    
    lexer_advance(lexer); // Skip opening quote
    
    while (lexer->current_char != '"' && 
           lexer->current_char != '\0' && 
           lexer->current_char != '\n' &&
           i < STRING_SIZE-1)
    {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    if (lexer->current_char != '"')
    {
        buffer[i] = '\0';
        return lexer_report_error(lexer,
                                  str_line,
                                  str_column,
                                  "missing terminating \" character: %s",
                                  buffer);
    }
    
    buffer[i] = '\0';
    lexer_advance(lexer); // Skip closing quote
 
    Token token;
    memset(&token, 0, sizeof(token));

    token.type = TOKEN_STRING;

    strcpy(token.text, "\""); // For debug, keep quotes in text
    strcat(token.text, buffer);
    strcat(token.text, "\"");
    
    // Store content without quotes in string_value field
    strcpy(token.value.string, buffer);
    
    return token;
}


// ============================================
// Implementation of Public Functions
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

        // COMMENTS
        if (lexer->current_char == '#')
        {
            // Skip everything until end of line BUT DON'T consume the \n
            while (lexer->current_char != '\n' && 
                   lexer->current_char != '\0')
            {
                lexer_advance(lexer);
            }
            // DON'T consume the \n! It will be returned as TOKEN_EOL
            // Continue in loop to check for more spaces/comments
            continue;
        }
        // If not a comment, break from loop and process token
        break;
    }

    // NOW process token normally
    Token token;
    memset(&token, 0, sizeof(token));

    int line = lexer->line;
    int column = lexer->column;        
    char c = lexer->current_char;
    
    // End of file
    if (c == '\0')
    {
        token.type = TOKEN_EOF;
        token.line = line;
        token.column = column;

        return token;
    }
    
    // Numbers
    if (isdigit(c) || c == '.')
    {
        return lexer_read_number(lexer);
    }
    
    // Identifiers
    if (isalpha(c) || c == '_')
    {
        return lexer_read_identifier(lexer);
    }

    if(c == '"')
    {
        return lexer_read_string(lexer);
    }

    // Operators and special characters
    switch (c)
    {
        case '+':
            lexer_advance(lexer);
            token.type = TOKEN_PLUS;
            strcpy(token.text, "+");
            token.line = line;
            token.column = column;
            return token;

        case '-':
            lexer_advance(lexer);
            token.type = TOKEN_MINUS;
            strcpy(token.text, "-");
            token.line = line;
            token.column = column;
            return token;
            
        case '*':
            lexer_advance(lexer);
            token.type = TOKEN_STAR;
            strcpy(token.text, "*");
            token.line = line;
            token.column = column;
            return token;

            
        case '/':
            lexer_advance(lexer);
            token.type = TOKEN_SLASH;
            strcpy(token.text, "/");
            token.line = line;
            token.column = column;
            return token;
            
        case '(':
            lexer_advance(lexer);
            token.type = TOKEN_LPAREN;
            strcpy(token.text, "(");
            token.line = line;
            token.column = column;
            return token;
            
        case ')':
            lexer_advance(lexer);
            token.type = TOKEN_RPAREN;
            strcpy(token.text, ")");
            token.line = line;
            token.column = column;
            return token;

        case '=':
        {
            if(lexer_peek_next(lexer) == '=')
            {
                token.type = TOKEN_EQUAL;
                strcpy(token.text, "==");
                token.line = line;
                token.column = column;
                lexer_advance(lexer); // Primeiro '='
                lexer_advance(lexer); // Segundo '='
                return token;
            }
            token.type = TOKEN_ASSIGN;
            strcpy(token.text, "=");
            token.line = line;
            token.column = column;
            lexer_advance(lexer);
            return token;             
        }

        case '!':
        {
            if(lexer_peek_next(lexer) == '=')
            {
                token.type = TOKEN_NOT_EQUAL;
                strcpy(token.text, "!=");
                token.line = line;
                token.column = column;
                lexer_advance(lexer); 
                lexer_advance(lexer); 
                return token;
            } 
            token.type = TOKEN_NOT;
            strcpy(token.text, "!");
            token.line = line;
            token.column = column;
            lexer_advance(lexer);
            return token;         
        }

        case '<':
        {
            if(lexer_peek_next(lexer) == '=')
            {
                token.type = TOKEN_LESS_EQUAL;
                strcpy(token.text, "<=");
                token.line = line;
                token.column = column;
                lexer_advance(lexer); 
                lexer_advance(lexer); 
                return token;
            } 
            token.type = TOKEN_LESS;
            strcpy(token.text, "<");
            token.line = line;
            token.column = column;
            lexer_advance(lexer);
            return token; 
        }  

        case '>':
        {
            if(lexer_peek_next(lexer) == '=')
            {
                token.type = TOKEN_GREATER_EQUAL;
                strcpy(token.text, ">=");
                token.line = line;
                token.column = column;
                lexer_advance(lexer); 
                lexer_advance(lexer); 
                return token;
            } 
            token.type = TOKEN_GREATER;
            strcpy(token.text, ">");
            token.line = line;
            token.column = column;
            lexer_advance(lexer);
            return token; 
        } 

        case '\n':
            lexer_advance(lexer);
            token.type = TOKEN_EOL;
            strcpy(token.text, "EOL");
            token.line = line;
            token.column = column;
            return token; 

        case ':':
            lexer_advance(lexer);
            token.type = TOKEN_COLON;
            strcpy(token.text, ":");
            token.line = line;
            token.column = column;
            return token;

        case ';':
            lexer_advance(lexer);
            token.type = TOKEN_SEMICOLON;
            strcpy(token.text, ";");
            token.line = line;
            token.column = column;
            return token;

        case '?':
            lexer_advance(lexer);
            token.type = TOKEN_QUESTION;
            strcpy(token.text, "?");
            token.line = line;
            token.column = column;
            return token;
            
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
    printf("(%d:%d)", token.line, token.column);
    printf("[%s]", TOKEN_STRINGS[token.type]);
    
    if (token.type == TOKEN_NUMBER)
    {
        printf(": %g", token.value.number);
        if (token.text[0] != '\0')
        {
            printf(" (text: %s)", token.text);
        }
    }
    else if (token.type == TOKEN_STRING)
    {
        printf(": %s", token.value.string);
    }
    else if (token.type == TOKEN_IDENTIFIER)
    {
        printf(": %s", token.value.varname);
    }
    else if (token.type == TOKEN_ERROR)
    {
        printf("%s\n%s\n%s", COLOR_ERROR, token.text, COLOR_RESET);
    }
    // keywords, se não estiverem nos else if acima, caem aqui
    else if (token.text[0] != '\0')
    {
        printf(": %s", token.text);
    }
}

void lexer_print_all_tokens(const char* source)
{
    printf("%s=== LEXICAL ANALYSIS ===\n%s", COLOR_HEADER, COLOR_RESET);
    printf("Source code: \"%s\"\n\n", source);
    
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
        printf(COLOR_WARNING "\nAnalysis interrupted due to lexical error.\n" COLOR_RESET);
    }
    
    printf("\nTotal tokens: %d\n", token_count);
    printf("=== END OF ANALYSIS ===\n");
}

#ifdef TESTLEXER
#include "color.h"
#include "utils.h"

int main()
{
    setup_utf8();
    
    printf("ZzBasic Lexer Test v0.5.1 - 'true' e 'false'\n\n");
    lexer_print_all_tokens("and or not = == ! != < <= > >= \n");
    //wait(); 
    
    return 0;
}
#endif

// Fim de lexer.c