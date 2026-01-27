// parser.c

#include "color.h"
#include "ast.h"
#include "parser.h"
#include "a89alloc.h"

//===================================================================
// PROTÓTIPOS DAS FUNÇÕES DESTE ARQUIVO
//===================================================================
static void parser_init(Parser* parser, Lexer* lexer);
static void parser_advance(Parser* parser);
static int parser_expect(Parser* parser, TokenType expected_type);
static void parser_set_error(Parser* parser, const char* message);

static int is_keyword_token(TokenType type);
static const char* get_keyword_name(TokenType type);
static void report_print_keyword_error(Parser* parser, Token token);

static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_statement_list(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_assignment_stmt(Parser* parser);
static ASTNode* parse_print_statement(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_expression_stmt(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_atom(Parser* parser);
//===================================================================


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
    
    // Formata a mensagem com cor vermelha
    char formatted_message[BUFFER_SIZE];
    snprintf(formatted_message, sizeof(formatted_message),
             "%s%s%s", COLOR_ERROR, message, COLOR_RESET);
    
    // Copia para o buffer do parser
    strncpy(parser->error_message, formatted_message, 
            sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}

// static int is_string_variable(const char* name) {
//     int len = strlen(name);
//     return (len > 0 && name[len - 1] == '$');
// }

// =====================================================================
// FUNÇÕES AUXILIARES PARA MANIPULAÇÃO DE PALAVRAS-CHAVE
// =====================================================================

// Verifica se um token é palavra-chave/comando
static int is_keyword_token(TokenType type) {
    switch (type) {
        case TOKEN_LET:      // Comando let
        case TOKEN_PRINT:    // Comando print
        case TOKEN_QUESTION: // Atalho ?
        // FUTURO: adicionar novos comandos aqui
        // case TOKEN_IF:
        // case TOKEN_FOR:
        // case TOKEN_WHILE:
        // case TOKEN_FUNCTION:
        // case TOKEN_RETURN:
            return 1;  // É palavra-chave/comando
        default:
            return 0;  // Não é palavra-chave
    }
}

// Obtém nome amigável de uma palavra-chave
static const char* get_keyword_name(TokenType type) {
    switch (type) {
        case TOKEN_LET:      return "let";
        case TOKEN_PRINT:    return "print";
        case TOKEN_QUESTION: return "?";
        // FUTURO: adicionar novos comandos aqui
        // case TOKEN_IF:       return "if";
        // case TOKEN_FOR:      return "for";
        default:             return "command";
    }
}

// Gera mensagem de erro específica para token problemático em print
static void report_print_keyword_error(Parser* parser, Token token) {
    char error_msg[BUFFER_SIZE];
    
    if (token.type == TOKEN_SEMICOLON) {
        snprintf(error_msg, sizeof(error_msg),
            "Error [%d:%d]: print statement cannot have ';' after it. "
            "Remove the ';' or use: print \"text1\" \"text2\" nl",
            token.line, token.column);
    }
    else if (token.type == TOKEN_COLON) {
        snprintf(error_msg, sizeof(error_msg),
            "Error [%d:%d]: print statement cannot have ':' after it. "
            "Use new line for next statement.",
            token.line, token.column);
    }
    else if (is_keyword_token(token.type)) {
        const char* keyword_name = get_keyword_name(token.type);
        
        // Mensagens específicas por tipo de comando
        if (token.type == TOKEN_LET) {
            snprintf(error_msg, sizeof(error_msg),
                "Error [%d:%d]: '%s' is a command, not a valid expression. "
                "Assign variables BEFORE printing:\n"
                "  let x = 5\n"
                "  print x nl",
                token.line, token.column, keyword_name);
        }
        else if (token.type == TOKEN_PRINT || token.type == TOKEN_QUESTION) {
            snprintf(error_msg, sizeof(error_msg),
                "Error [%d:%d]: '%s' is a command, not a valid expression. "
                "Use ONE print with multiple items:\n"
                "  print \"text1\" \"text2\" nl  (instead of print \"text1\" print \"text2\")",
                token.line, token.column, keyword_name);
        }
        else {
            // Mensagem genérica para outros comandos (futuro)
            snprintf(error_msg, sizeof(error_msg),
                "Error [%d:%d]: '%s' is a command, not a valid expression in print statement.",
                token.line, token.column, keyword_name);
        }
    }
    else {
        // Erro genérico para outros tokens inesperados
        snprintf(error_msg, sizeof(error_msg),
            "Error [%d:%d]: Unexpected '%s' in print statement",
            token.line, token.column, token.text[0] ? token.text : "token");
    }
    
    parser_set_error(parser, error_msg);
}


//===================================================================
// PARSING FUNCTIONS
//===================================================================

//===================================================================
// program := (statement | comment)* EOF
//===================================================================
static ASTNode* parse_program(Parser* parser)
{
    return parse_statement_list(parser);
}

static ASTNode* parse_statement_list(Parser* parser)
{
    ASTNode* list = create_statement_list_node(
        parser->current_token.line,
        parser->current_token.column
    );
    
    // Parseia o primeiro statement
    ASTNode* stmt = parse_statement(parser);
    if (!stmt || parser->has_error) {
        free_ast(list);
        return NULL;
    }
    statement_list_add(list, stmt);
    
    // Continua parseando enquanto encontrar separadores
    while (!parser->has_error && 
           parser->current_token.type != TOKEN_EOF)
    {
        
        Token token = parser->current_token;
        
        // Verifica se é um separador
        if (token.type == TOKEN_COLON || 
            token.type == TOKEN_SEMICOLON ||
            token.type == TOKEN_EOL)
        {
            
            parser_advance(parser);  // Consome o separador
            
            // Se após separador for EOF ou EOL, para (statements opcionais)
            if (parser->current_token.type == TOKEN_EOF ||
                parser->current_token.type == TOKEN_EOL)
            {
                // Permite separadores no final (BASIC tradicional)
                break;
            }

            // Parseia próximo statement
            stmt = parse_statement(parser);
            if (!stmt || parser->has_error)
            {
                free_ast(list);
                return NULL;
            }
            statement_list_add(list, stmt);
        }
        else
        {
            // Não é separador, sai do loop
            break;
        }
    }
    
    return list;
}

//===================================================================
// statement := assignment_stmt | print_stetemant | expression_stmt
//===================================================================
static ASTNode* parse_statement(Parser* parser)
{
    if (parser->current_token.type == TOKEN_LET)
    {
        return parse_assignment_stmt(parser);
    } 
    else if (parser->current_token.type == TOKEN_PRINT || 
             parser->current_token.type == TOKEN_QUESTION)
    {
        return parse_print_statement(parser);  
    }
    else
    {
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
    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        // Se for palavra-chave, dá erro específico
        if (is_keyword_token(parser->current_token.type))
        {
            const char* keyword_name = get_keyword_name(parser->current_token.type);
            char error_msg[BUFFER_SIZE];
            snprintf(error_msg, sizeof(error_msg),
                "Error [%d:%d]: '%s' is a command keyword, cannot be used as variable name",
                parser->current_token.line, parser->current_token.column, keyword_name);
            parser_set_error(parser, error_msg);
        }
        else
        {
            parser_set_error(parser, "Error: expected identifier");
        }
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
// print_stmt := ('print' | '?') print_item* (nl | EOL | EOF)
// print_item := expression
//===================================================================
static ASTNode* parse_print_statement(Parser* parser)
{
    // Salva posição do comando print/?
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    // Consome o token print ou ?
    parser_advance(parser);
    
    // Cria nó do comando print
    ASTNode* print_node = create_print_node(line, column);
    if (!print_node)
    {
        parser_set_error(parser, "Error: could not create print node");
        return NULL;
    }
    
    // Parseia os itens (expressões)
    while (!parser->has_error)
    {
        Token token = parser->current_token;
        
        // PARA nestes casos:
        if (token.type == TOKEN_NL  ||
            token.type == TOKEN_EOL ||
            token.type == TOKEN_EOF)
        {
            break;
        }
        
        // Verifica tokens problemáticos: ;, :, ou palavras-chave
        if (token.type == TOKEN_SEMICOLON ||
            token.type == TOKEN_COLON ||
            is_keyword_token(token.type))
        {
            report_print_keyword_error(parser, token);
            free_ast(print_node);
            return NULL;
        }
        
        // Agora parseia com segurança
        ASTNode* item = parse_expression(parser);
        if (parser->has_error) {
            free_ast(print_node);
            return NULL;
        }
        
        print_node_add_item(print_node, item);
    }

    // Verifica se tem 'nl' no final
    if (parser->current_token.type == TOKEN_NL) {
        print_set_newline(print_node, 1);  // 1 = tem nl (quebra linha)
        parser_advance(parser);  // Consome o 'nl'
    } else {
        print_set_newline(print_node, 0);  // 0 = sem nl (mesma linha)
    }


    
    return print_node;
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
static ASTNode* parse_atom(Parser* parser)
{
    Token token = parser->current_token;
    
    printf("Token atual: ");
    lexer_print_token(parser->current_token);

    switch (token.type)
    {
        case TOKEN_NUMBER:
            parser_advance(parser);
            return create_number_node(token.value.number,token.line, token.column);

        case TOKEN_STRING:
            parser_advance(parser);
            return create_string_node(token.value.string,token.line, token.column);
            
        case TOKEN_IDENTIFIER:
            parser_advance(parser);
            return create_variable_node(token.value.varname, token.line, token.column);
            
        case TOKEN_LPAREN:
        {
            parser_advance(parser);  // Consume '('
            ASTNode* node = parse_expression(parser);
            if (parser->has_error) return NULL;
            
            if (!parser_expect(parser, TOKEN_RPAREN))
            {
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
ASTNode* parse(Lexer* lexer)
{
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF) {
        return NULL;
    }
    
    ASTNode* result = parse_program(&parser);
    
    if (parser.has_error)
    {
        if (result != NULL)
        {
            free_ast(result);
        }
        printf("%s\n", parser.error_message);
        return NULL;
    }

    // Pula EOLs finais antes de verificar EOF
    while (parser.current_token.type == TOKEN_EOL)
    {
        parser_advance(&parser);
    }
    
    if (parser.current_token.type != TOKEN_EOF)
    {
        if (result != NULL)
        {
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
    
    printf("%s=== TESTE PARSER v0.3.0 (Múltiplos Statements) ===%s\n\n", 
           COLOR_HEADER, COLOR_RESET);
    
    char* testes[] = {
        // ============================================
        // TESTES EXISTENTES (compatibilidade)
        // ============================================
        "-5 + 3",
        "10 - 4 * 2",
        "-(5 + 3) * 2",
        "10 / 2 + 1",
        
        "let x = 10",
        "let y = 20",
        "x + y",
        "let pi = 3.14159",
        "pi * 2",
        "let total = (x + y) * 2",
        "total / 3",
        "let a = -5",
        "a + 10",
        
        // ============================================
        // NOVOS TESTES v0.3.0 - MÚLTIPLOS STATEMENTS
        // ============================================
        
        // 1. Dois-pontos (BASIC tradicional)
        "let a = 5 : let b = a * 2",
        "let x = 10 : let y = 20 : x + y",
        "let nome = \"Zz\" : let versao = 0.3 : nome",
        
        // 2. Ponto-e-vírgula (moderno)
        "let a = 5; let b = a * 2",
        "let x = 10; let y = 20; x + y;",
        "let a = 1; let b = 2; let c = 3; a + b + c",
        
        // 3. Misturado
        "let a = 5 : let b = 10; a + b",
        "let x = 1; let y = 2 : let z = 3; x * y * z",
        
        // 4. Com strings
        "let msg = \"Olá\" : let alvo = \"Mundo\" : msg",
        "let a = \"teste\"; let b = 123; a",
        
        // 5. Expressões complexas com múltiplos statements
        "let base = 10 : let altura = 5 : let area = (base * altura) / 2 : area",
        
        // 6. Apenas um statement (com separador no final) - deve funcionar
        "let x = 100;",
        "let y = 200:",
        
        // 7. Testes de erro (esperados)
         "let x = 5 : : let y = 10",  // Dois separadores seguidos - ERRO
         "let x = 5 ; ; ;",           // Múltiplos separadores - ERRO
         ": let x = 5",               // Separador no início - ERRO
    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++) {
        printf("%s=== Teste %d: '%s' ===%s\n", 
               COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        // USAR parse (não parse_single_statement) para testar múltiplos statements
        ASTNode* ast = parse(&lexer);
        
        if (ast) {
            printf("AST gerada:\n");
            print_ast(ast, 0);
            free_ast(ast);
            printf("%s✓ Parsing OK%s\n", COLOR_SUCCESS, COLOR_RESET);
        } else {
            printf("%s✗ ERRO no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        printf("\n");
        wait();
    }
    
    // ============================================
    // TESTES ESPECIAIS
    // ============================================
    printf("%s=== TESTES ESPECIAIS ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Teste com múltiplas linhas em uma string (simulando arquivo)
    printf("\n%sTeste com múltiplas linhas:%s\n", YELLOW, COLOR_RESET);
    {
        char* multi_line = "let x = 5\nlet y = 10\nx + y";
        printf("Código: \"%s\"\n", multi_line);
        
        Lexer lexer;
        lexer_init(&lexer, multi_line);
        
        ASTNode* ast = parse(&lexer);
        if (ast) {
            printf("AST:\n");
            print_ast(ast, 0);
            free_ast(ast);
            printf("%s✓ OK%s\n", COLOR_SUCCESS, COLOR_RESET);
        } else {
            printf("%s✗ ERRO%s\n", COLOR_ERROR, COLOR_RESET);
        }
    }
    
    wait();
    
    // Teste de separadores no final (deve ser permitido)
    printf("\n%sTeste separadores no final:%s\n", YELLOW, COLOR_RESET);
    {
        char* tests[] = {
            "let x = 5:",
            "let y = 10;",
            "let z = 15:\n",
            "let w = 20;\n",
        };
        
        for (int i = 0; i < 4; i++) {
            printf("  '%s': ", tests[i]);
            Lexer lexer;
            lexer_init(&lexer, tests[i]);
            
            ASTNode* ast = parse(&lexer);
            if (ast) {
                printf("%sOK%s\n", COLOR_SUCCESS, COLOR_RESET);
                free_ast(ast);
            } else {
                printf("%sERRO%s\n", COLOR_ERROR, COLOR_RESET);
            }
        }
    }
    
    printf("\n%s=== TODOS OS TESTES COMPLETADOS ===%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    a89check_leaks();
    return 0;
}
#endif
// END OF parser.c