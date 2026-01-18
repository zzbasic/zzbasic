#include "parser.h"

#include "a89alloc.h"


// ====================
// FUNÇÕES DO PARSER
// ====================
void parser_init(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->has_error = 0;
    strcpy(parser->error_message, "");
}

void parser_advance(Parser* parser) {
    parser->current_token = lexer_get_next_token(parser->lexer);
}

int parser_expect(Parser* parser, Token_type expected_type) {
    return parser->current_token.type == expected_type;
}

void parser_set_error(Parser* parser, const char* message) {
    parser->has_error = 1;
    strncpy(parser->error_message, message, sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}


// =========================
// CRIACAO DE NOS DA AST
// =========================
ASTNode* create_number_node(double value) {
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    //ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Erro ao alocar memória para number_node: %.2f\n", value);
        exit(EXIT_FAILURE);
    }
    node->type = NODE_NUMBER;
    node->value = value;
    node->operator = '\0'; 
    node->left = node->right = node->operand = NULL;
    return node;
}

ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right) {
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    //ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Erro ao alocar memória para binary_op_node: %c\n", operator);
        exit(EXIT_FAILURE);
    }
    node->type = NODE_BINARY_OP;
    node->value = 0;
    node->operator = operator;
    node->left = left;
    node->right = right;
    node->operand = NULL;
    return node;
}

ASTNode* create_unary_op_node(char operator, ASTNode* operand) {
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    //ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        printf("Erro ao alocar memória para unary_op_node: %c\n", operator);
        exit(EXIT_FAILURE);
    }
    node->type = NODE_UNARY_OP;

    node->value = 0;
    node->operator = operator;
    node->left = node->right = NULL;
    node->operand = operand;
    return node;
}

// ===========================
// LIBERANDO A MEMÓRIA ALOCADA
// ===========================
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->operand);    
    a89free(node);
    //free(node);
}

/********************************************************************
FUNCOES DE PARSING 

GRAMATICA v0.1.0

expression       := arithmetic_expr | E
arithmetic_expr  := term (('+' | '-') term)*
term             := factor (('*' | '/') factor)*
factor           := NUMBER | '(' expression ')' |
                    '-' (NUMBER | '(' expression ')')

********************************************************************/

//===================================================================
// expression := arithmetic_expr | E
//===================================================================
ASTNode* parse_expression(Parser* parser) {
    return parse_arithmetic_expr(parser);
}


//===================================================================
// arithmetic_expr := term (('+' | '-') term)*
//===================================================================
ASTNode* parse_arithmetic_expr(Parser* parser) {
    ASTNode* node = parse_term(parser);
    if (parser->has_error) return NULL;    
    while (parser->current_token.type == TOKEN_OPERATOR &&
           (parser->current_token.operator == '+' ||
            parser->current_token.operator == '-')) {
        char op = parser->current_token.operator;
        parser_advance(parser);
        ASTNode* right = parse_term(parser);
        if (parser->has_error) {
            free_ast(node);
            return NULL;
        }
        node = create_binary_op_node(op, node, right);
    }    
    return node;
}

//===================================================================
// term := factor (('*' | '/') factor)*
//===================================================================
ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    if (parser->has_error) return NULL;
    
    while (parser->current_token.type == TOKEN_OPERATOR &&
           (parser->current_token.operator == '*' || 
            parser->current_token.operator == '/' )) {
        char op = parser->current_token.operator;
        parser_advance(parser);
        ASTNode* right = parse_factor(parser);
        if (parser->has_error) {
            free_ast(node);
            return NULL;
        }
        node = create_binary_op_node(op, node, right);
    }
    
    return node;
}

//===================================================================
// factor := NUMBER | '(' expression ')' | '-' expression
//===================================================================
ASTNode* parse_factor(Parser* parser) {
    Token token = parser->current_token;
    switch (token.type) {
        case TOKEN_NUMBER:
            parser_advance(parser);
            return create_number_node(token.value);            
        case TOKEN_LPAREN:
            parser_advance(parser);
            ASTNode* node = parse_expression(parser);
            if (parser->has_error) return NULL;            
            if (!parser_expect(parser, TOKEN_RPAREN)) {
                free_ast(node);
                strcpy(parser->error_message, "Erro: Falta o parêntese da direita.\n");
                return NULL;
            }
            parser_advance(parser);
            return node;
            
        case TOKEN_OPERATOR:
            if (token.operator == '-') {
                parser_advance(parser);
                ASTNode* operand = parse_expression(parser);
                if (parser->has_error) return NULL;
                return create_unary_op_node('-', operand);
            }
            break;

        case TOKEN_ERROR:
            strcpy(parser->error_message, parser->current_token.text);
            return NULL;
        default:
            strcpy(parser->error_message, "Erro: token desconhecido.\n");
            break;
    }
    
    strcpy(parser->error_message, "Erro: expressão inválida.\n");
    return NULL;
}

//================================
// Função PRINCIPAL DE PARSING
//================================
ASTNode* parse(Lexer* lexer) {
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF) {
        return NULL;
    }
    
    ASTNode* result = parse_expression(&parser);
    
    if (parser.has_error) {
        if (result != NULL) {
            free_ast(result);
        }
        printf("%s\n", parser.error_message);
        return NULL;
    }
    
    if (parser.current_token.type != TOKEN_EOF) {
        if (result != NULL) {
            free_ast(result);
        }
        printf("Erro: expressão incompleta.\n");
        return NULL;
    }
    
    return result;
}

void print_ast(ASTNode* node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("    ");
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %.2f\n", node->value);
            break;
        case NODE_BINARY_OP:
            printf("BINARY_OP: %c\n", node->operator);
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
        case NODE_UNARY_OP:
            printf("UNARY_OP: %c\n", node->operator);
            print_ast(node->operand, indent + 1);
            break;
    }
}
