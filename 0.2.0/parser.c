// parser.c

#include "ast.h"
#include "parser.h"
#include "a89alloc.h"

//=============================
// PARSER HELPER FUNCTIONS
//=============================
static void parser_init(Parser* parser, Lexer* lexer)
{
    parser->lexer = lexer;
    memset(&parser->current_token, 0, sizeof(Token));  
    parser->current_token = lexer_get_next_token(lexer);
    parser->has_error = 0;
    parser->error_message[0] = '\0';
}

static void parser_advance(Parser* parser)
{
    parser->current_token = lexer_get_next_token(parser->lexer);
}

static int parser_expect(Parser* parser, TokenType expected_type)
{
    return parser->current_token.type == expected_type;
}

// static int parser_expect_operator(Parser* parser, char op) {
//     return (parser->current_token.type == TOKEN_OPERATOR && 
//             parser->current_token.operator == op);
// }

static void parser_set_error(Parser* parser, const char* message)
{
    parser->has_error = 1;
    strncpy(parser->error_message, message, sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}

// static int is_string_variable(const char* name) {
//     int len = strlen(name);
//     return (len > 0 && name[len - 1] == '$');
// }


//===================================================================
// PARSING FUNCTIONS
//===================================================================
static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_assignment_stmt(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);

static ASTNode* parse_expression_stmt(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_atom(Parser* parser);

//===================================================================
// program := (statement | comment)* EOF
//===================================================================
static ASTNode* parse_program(Parser* parser)
{
    // Comments already handled in Lexer

    ASTNode* last_expression = NULL;
    
    while (!parser->has_error && 
           parser->current_token.type != TOKEN_EOF)
    {
        
        ASTNode* stmt = parse_statement(parser);
        
        if (parser->has_error) {
            if (stmt) free_ast(stmt);
            return NULL;
        }
        
        // Stores the last expression (will be evaluated)
        // Assignments are not evaluable expressions
        if (stmt && stmt->type != NODE_ASSIGNMENT && 
                    stmt->type != NODE_STRING)
        {
            if (last_expression) free_ast(last_expression);
            last_expression = stmt;
        }
        else if (stmt)
        {
            // Assignment or string - not evaluable expression
            free_ast(stmt);
        }
    }
    
    return last_expression;  // Returns last expression for evaluation
}

//===================================================================
// statement := assignment_stmt | expression_stmt
//===================================================================
static ASTNode* parse_statement(Parser* parser) {
    if (parser->current_token.type == TOKEN_LET) {
        return parse_assignment_stmt(parser);
    } else {
        return parse_expression_stmt(parser);
    }
}


//===================================================================
// assignment_stmt := LET identifier '=' expression
//===================================================================
static ASTNode* parse_assignment_stmt(Parser* parser)
{
    // We already verified it's TOKEN_LET before calling this function
    parser_advance(parser);  // Consume LET
    
    // Check identifier
    if (parser->current_token.type != TOKEN_IDENTIFIER) {
        parser_set_error(parser, "Error: expected identifier");
        return NULL;
    }

    char var_name[VARNAME_SIZE];
    strncpy(var_name, parser->current_token.value.varname, VARNAME_SIZE - 1);
    var_name[VARNAME_SIZE - 1] = '\0';

    parser_advance(parser);  // Consume identifier
    
     // Check '='
    if (parser->current_token.type != TOKEN_EQUAL) {
        parser_set_error(parser, "Error: Expected '=' after variable name");
        return NULL;
    }
    
    parser_advance(parser);  // Consume '='

    // For string variables: expect STRING_LITERAL
    if(parser->current_token.type == TOKEN_STRING)
    {
        ASTNode* string_node = create_string_node(parser->current_token.value.string,
                                                  parser->current_token.line,
                                                  parser->current_token.column);

        parser_advance(parser);  // Consume STRING_LITERAL

        return create_assignment_node(var_name, string_node,
                                      string_node->line,
                                      string_node->column);
    }

    ASTNode* expr = parse_expression(parser);
    if (parser->has_error) {
        return NULL;
    }

    return create_assignment_node(var_name, expr, expr->line, expr->column);
}

//===================================================================
// expression_stmt := expression
//===================================================================
static ASTNode* parse_expression_stmt(Parser* parser) {
    return parse_expression(parser);
}

//===================================================================
// expression := term (('+' | '-') term)*
//===================================================================
static ASTNode* parse_expression(Parser* parser) {
    ASTNode* node = parse_term(parser);
    if (parser->has_error) return NULL;
    
    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS) {
        char op;
        if(parser->current_token.type == TOKEN_PLUS)
        {
            op = '+';
        }
        else
        {
            op = '-';
        }
        parser_advance(parser);

        ASTNode* right = parse_term(parser);
        if (parser->has_error) {
            free_ast(node);
            return NULL;
        }
        
        node = create_binary_op_node(op, node, right, node->line, node->column);
    }
    
    return node;
}

//===================================================================
// term := factor (('*' | '/') factor)*
//===================================================================
static ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    if (parser->has_error) return NULL;
    
    while (parser->current_token.type == TOKEN_STAR ||
           parser->current_token.type == TOKEN_SLASH)
    {
        char op;
        if(parser->current_token.type == TOKEN_STAR)
        {
            op = '*';
        }
        else
        {
            op = '/';
        }
        parser_advance(parser);

        ASTNode* right = parse_factor(parser);
        if (parser->has_error) {
            free_ast(node);
            return NULL;
        }
        node = create_binary_op_node(op, node, right, node->line, node->column);
    }
    
    return node;
}

//===================================================================
// factor          := ('+' | '-')? atom
//===================================================================
static ASTNode* parse_factor(Parser* parser)
{
    char op = '+';
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    // Only advance if it's + or -
    if (parser->current_token.type == TOKEN_PLUS || 
        parser->current_token.type == TOKEN_MINUS)
    {
        
        if (parser->current_token.type == TOKEN_MINUS)
        {
            op = '-';
        }
        parser_advance(parser);
    }
    
    ASTNode* operand = parse_atom(parser);
    if (parser->has_error) return NULL;
    
    // If there was no unary operator, return directly
    if (op == '+') {
        return operand;  // +x is the same as x
    }
    
    return create_unary_op_node(op, operand, line, column);
}


//===================================================================
// atom := NUMBER | STRING | identifier | '(' expression ')' 
//===================================================================
static ASTNode* parse_atom(Parser* parser){
    Token token = parser->current_token;
    
    switch (token.type) {
        case TOKEN_NUMBER:
            parser_advance(parser);
            return create_number_node(token.value.number,token.line, token.column);

        case TOKEN_STRING:
            parser_advance(parser);
            return create_string_node(token.value.string,token.line, token.column);
            
        case TOKEN_IDENTIFIER:
            parser_advance(parser);
            return create_variable_node(token.value.varname, token.line, token.column);
            
        case TOKEN_LPAREN: {
            parser_advance(parser);  // Consume '('
            ASTNode* node = parse_expression(parser);
            if (parser->has_error) return NULL;
            
            if (!parser_expect(parser, TOKEN_RPAREN)) {
                free_ast(node);
                parser_set_error(parser, "Error: Expected ')'");
                return NULL;
            }
            parser_advance(parser);  // Consume ')'
            return node;
        }
            
        default:
            parser_set_error(parser, "Error: Unexpected factor");
            return NULL;
    }

    return NULL;
}

//================================
// MAIN PARSING FUNCTION
//================================
ASTNode* parse(Lexer* lexer) {
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF) {
        return NULL;
    }
    
    ASTNode* result = parse_program(&parser);
    
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
        printf("Error: incomplete expression.\n");
        return NULL;
    }
    
    return result;
}

//===================================================================
// FOR TESTING V0.2.0
//===================================================================
ASTNode* parse_single_statement(Lexer* lexer) {
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF) {
        return NULL;
    }
    
    // Parse ONLY ONE statement (can be assignment OR expression)
    ASTNode* result = parse_statement(&parser);
    
    if (parser.has_error) {
        if (result) free_ast(result);
        printf("%s\n", parser.error_message);
        return NULL;
    }
    
    // Check if everything was parsed
    if (parser.current_token.type != TOKEN_EOF) {
        printf("Warning: remaining tokens not parsed\n");
        // But still returns the result
    }
    
    return result;
}

#ifdef TESTPARSER
#include "color.h"
#include "utils.h"

int main() {
    setup_utf8();
    
    char* testes[] = {
        // Basic tests
        "-5 + 3",
        "10 - 4 * 2",
        "-(5 + 3) * 2",
        "10 / 2 + 1",
        
        // Variables and assignment
        "let x = 10",
        "let y = 20",
        "x + y",
        "let pi = 3.14159",
        "pi * 2",
        "let total = (x + y) * 2",
        "total / 3",
        "let a = -5",
        "a + 10",
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++) {
        printf("%s=== Teste %d: '%s' ===%s\n", 
               COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        ASTNode* ast = parse_single_statement(&lexer);
        
        if (ast) {
            printf("AST gerada:\n");
            print_ast(ast, 0);
            free_ast(ast);
        } else {
            printf("%sERRO no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        printf("\n");
        wait();
    }
    
    a89check_leaks();
    return 0;
}
#endif
// END OF parser.c