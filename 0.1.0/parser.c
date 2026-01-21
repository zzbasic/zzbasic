#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "a89alloc.h"

// ============================================
// PROTÓTIPOS das Funções Privadas (static)
// ============================================
static void parser_init(Parser* parser, Lexer* lexer);
static void parser_advance(Parser* parser);
static int parser_expect(Parser* parser, Token_type expected_type);
static void parser_set_error(Parser* parser, const char* message);
static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_atom(Parser* parser);

//============================================
// FUNÇÕES PÚBLICAS NO FINAL
//============================================
//ASTNode* parse(Lexer* lexer)



static void parser_init(Parser* parser, Lexer* lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->has_error = 0;
    strcpy(parser->error_message, "");
    // Inicializa posição com base no primeiro token
    parser->line = parser->current_token.line;
    parser->column = parser->current_token.column;
}

static void parser_advance(Parser* parser)
{
    parser->current_token = lexer_get_next_token(parser->lexer);
}

static int parser_expect(Parser* parser, Token_type expected_type)
{
    return parser->current_token.type == expected_type;
}

static void parser_set_error(Parser* parser, const char* message)
{
    char buffer[BUFFER_SIZE];
    parser->has_error = 1;
    snprintf(buffer, sizeof(buffer), "%s(%d, %d)%s%s",
        COLOR_ERROR,
        parser->line,
        parser->column,
        message,
        COLOR_RESET);
    strncpy(parser->error_message, buffer, sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}


/********************************************************************
FUNCOES DE PARSING 

GRAMATICA v0.1.0

program     := expression |  E
expression  := term (('+' | '-') term)*
term        := factor (('*' | '/') factor)*
factor      := NUMBER | '(' expression ')' | '-' (NUMBER | '(' expression ')')

********************************************************************/

//===================================================================
// program     := expression |  E
//===================================================================
static ASTNode* parse_program(Parser* parser)
{
    return parse_expression(parser);
}

//===================================================================
// expression  := term (('+' | '-') term)*
//===================================================================
static ASTNode* parse_expression(Parser* parser)
{
    int line = parser->line;
    int column = parser->column;

    ASTNode* node = parse_term(parser);

    #ifdef DEBUG
    printf("parse_expression() - node\n");
    print_ast(node, 0);
    wait();
    #endif

    if (parser->has_error) return NULL;    
    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS)
    {
        char op = parser->current_token.value.operator_char;
        parser_advance(parser);
        ASTNode* right = parse_term(parser);

        #ifdef DEBUG
        printf("parse_expression() - right\n");
        print_ast(node, 0);
        wait();
        #endif

        if (parser->has_error)
        {
            free_ast(node);
            return NULL;
        }
        node = create_binary_op_node(op, node, right, line, column);
    }    
    return node;
}

//===================================================================
// term := factor (('*' | '/') factor)*
//===================================================================
static ASTNode* parse_term(Parser* parser)
{
    int line = parser->line;
    int column = parser->column;

    ASTNode* node = parse_factor(parser);

    #ifdef DEBUG
    printf("parse_term() - node\n");
    print_ast(node, 0);
    wait();
    #endif

    if (parser->has_error) return NULL;    
    while (parser->current_token.type == TOKEN_STAR ||
           parser->current_token.type == TOKEN_SLASH)
    {
        char op = parser->current_token.value.operator_char;
        parser_advance(parser);
        ASTNode* right = parse_factor(parser);

        #ifdef DEBUG
        printf("parse_term() - right\n");
        print_ast(node, 0);
        wait();
        #endif

        if (parser->has_error)
        {
            free_ast(node);
            return NULL;
        }
        node = create_binary_op_node(op, node, right, line, column);
    }
    
    return node;
}

//===================================================================
// factor := ('+' | '-')? atom
//===================================================================
static ASTNode* parse_factor(Parser* parser)
{
    int line = parser->line;
    int column = parser->column;

    // Verifica operadores unários (+ ou -)
    int is_unary_minus = 0;
    
    if (parser->current_token.type == TOKEN_PLUS ||
        parser->current_token.type == TOKEN_MINUS)
    {
        // Guarda se é menos unário
        if (parser->current_token.type == TOKEN_MINUS) {
            is_unary_minus = 1;
        }
        // Consome o operador unário (+ ou -)
        parser_advance(parser);
    }
    
    // Parsing do atom (número ou expressao entre parenteses)
    ASTNode* node = parse_atom(parser);

    #ifdef DEBUG
    printf("parse_factor() - node\n");
    print_ast(node, 0);
    wait();
    #endif

    if (parser->has_error || node == NULL) {
        return NULL;
    }
    
    // Se era um 'menos unário', cria o nó unário
    if (is_unary_minus) {
        return create_unary_op_node('-', node, line, column);
    }
    
    // Se era um 'mais unário', retorna o nó diretamente
    // (operador + unário pode ser ignorado)
    return node;    
}

//===================================================================
// atom        := (NUMBER | '(' expression ')')
//===================================================================
static ASTNode* parse_atom(Parser* parser)
{
    int line = parser->line;
    int column = parser->column;

    Token token = parser->current_token;
    switch (token.type)
    {
        case TOKEN_NUMBER:
            parser_advance(parser);
            return create_number_node(token.value.number_value, line, column);            
        case TOKEN_LPAREN:
            parser_advance(parser);
            ASTNode* node = parse_expression(parser);
            if (parser->has_error) return NULL;            
            if (!parser_expect(parser, TOKEN_RPAREN))
            {
                free_ast(node);
                parser_set_error(parser, "Error: missing right parenthesis");
                return NULL;
            }
            parser_advance(parser);
            return node;
            
        case TOKEN_ERROR:
            parser_set_error(parser, token.text);
            return NULL;
        default:
        {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, sizeof(buffer),
                "Error: expected number or '(', found '%s'",token.type_string);
            parser_set_error(parser, buffer);
        }
    }
    
    parser_set_error(parser, "Error: invalid expression");
    return NULL;
}

//================================
// Função PRINCIPAL DE PARSING
//================================
ASTNode* parse(Lexer* lexer)
{
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF)
    {
        return NULL;
    }
    
    if (parser.current_token.type == TOKEN_ERROR)
    {
        parser_set_error(&parser, parser.current_token.text);
        printf("%s\n", parser.error_message);
        return NULL;
    }

    ASTNode* result = parse_expression(&parser);
    
    #ifdef DEBUG
    printf("parser() - result\n");
    print_ast(result, 0);
    wait();
    #endif

    if (parser.has_error)
    {
        if (result != NULL)
        {
            free_ast(result);
        }
        printf("%s\n", parser.error_message);
        return NULL;
    }

    if (parser.current_token.type == TOKEN_ERROR)
    {
        parser_set_error(&parser, parser.current_token.text);
        if (result != NULL)
        {
            free_ast(result);
        }
        printf("%s\n", parser.error_message);
        return NULL;
    }
    
    if (parser.current_token.type != TOKEN_EOF)
    {
        if (result != NULL)
        {
            free_ast(result);
        }
        printf("%s(%d, %d)Error: incomplete or invalid expression\n%s",
               COLOR_ERROR,
               parser.line,
               parser.column,
               COLOR_RESET);
        return NULL;
    }
    
    return result;
}
