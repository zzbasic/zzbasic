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
static int is_color_token(TokenType type);
static const char* get_color_name(TokenType type);
static void report_print_keyword_error(Parser* parser, Token token);

static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_statement_list(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_assignment_stmt(Parser* parser);
static ASTNode* parse_print_statement(Parser* parser);
static ASTNode* parse_color_statement(Parser* parser);
static ASTNode* parse_input_statement(Parser* parser);

// if_stmt

static ASTNode* parse_expression_stmt(Parser* parser);

// EXPRESSÕES LÓGICAS
static ASTNode* parse_logical_expr(Parser* parser);
static ASTNode* parse_logical_or_expr(Parser* parser);
static ASTNode* parse_logical_and_expr(Parser* parser);
static ASTNode* parse_not_expr(Parser* parser);
static ASTNode* parse_comparison_expr(Parser* parser);

// EXPRESSÕES ARITMÉTICAS
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_atom(Parser* parser);

// ASTNode* parse(Lexer* lexer)
// ASTNode* parse_single_statement(Lexer* lexer) 
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
             "%s[%d:%d] %s%s",
             COLOR_ERROR,
             parser->current_token.line,
             parser->current_token.column,
             message,
             COLOR_RESET);
    
    // Copia para o buffer do parser
    strncpy(parser->error_message, formatted_message, 
            sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';


    // printf("current_token.type  : %d\n", parser->current_token.type);
    // printf("current_token.line  : %d\n", parser->current_token.line);
    // printf("current_token.column: %d\n", parser->current_token.column);
}

// static int is_string_variable(const char* name) {
//     int len = strlen(name);
//     return (len > 0 && name[len - 1] == '$');
// }


// Verifica se um token é palavra-chave/comando
static int is_keyword_token(TokenType type) {
    switch (type) {
        case TOKEN_LET:      
        case TOKEN_PRINT:   
        case TOKEN_QUESTION: // ?
        case TOKEN_INPUT:     
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
        case TOKEN_INPUT:    return "input";
        // FUTURO: adicionar novos comandos aqui
        // case TOKEN_IF:       return "if";
        // case TOKEN_FOR:      return "for";
        default:             return "command";
    }
}

static int is_color_token(TokenType type)
{
    // Verifica se está no intervalo das cores
    return (type >= TOKEN_NOCOLOR && type <= TOKEN_BGWHITE);
}

// Verifica se um token pode ser statement sozinho (nocolor)
static int is_color_statement_token(TokenType type) {
    return (type == TOKEN_NOCOLOR);  // Apenas nocolor pode ser statement
}

// Obtém nome amigável de uma cor
static const char* get_color_name(TokenType type)
{
    switch (type) {
        case TOKEN_NOCOLOR: return "nocolor";
        case TOKEN_BLACK:   return "black";
        case TOKEN_RED:     return "red";
        case TOKEN_GREEN:   return "green";
        case TOKEN_YELLOW:  return "yellow";
        case TOKEN_BLUE:    return "blue";
        case TOKEN_MAGENTA: return "magenta";
        case TOKEN_CYAN:    return "cyan";
        case TOKEN_WHITE:   return "white";
        case TOKEN_BRED:    return "bred";
        case TOKEN_BGREEN:  return "bgreen";
        case TOKEN_BYELLOW: return "byellow";
        case TOKEN_BBLUE:   return "bblue";
        case TOKEN_BMAGENTA: return "bmagenta";
        case TOKEN_BCYAN:   return "bcyan";
        case TOKEN_BWHITE:  return "bwhite";
        default:            return "color";
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

static int is_operator_token(TokenType type)
{
    return type == TOKEN_PLUS || type == TOKEN_MINUS || 
           type == TOKEN_STAR || type == TOKEN_SLASH;
}

static void report_unexpected_token_error(Parser* parser, const char* context)
{
    Token token = parser->current_token;
    char error_msg[BUFFER_SIZE];
    
    if (is_operator_token(token.type))
    {
        snprintf(error_msg, sizeof(error_msg),
            "Error: Operator    '%s ' cannot appear at this position in %s",
            token.text, context);
    }
    else if (token.type == TOKEN_ERROR)
    {
        // Já tem mensagem de erro do lexer
        strncpy(error_msg, token.text, sizeof(error_msg) - 1);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg),
            "Error: Unexpected  '%s ' in %s (expected number, string, identifier or '(  ')",
            token.text, context);
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

    // Pula EOLs iniciais (de comentários ou linhas em branco)
    while (parser->current_token.type == TOKEN_EOL)
    {
        parser_advance(parser);
    }

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

            // Pula EOLs adicionais
            while (parser->current_token.type == TOKEN_EOL)
            {
                parser_advance(parser);
            }
            
            
            // Se após separador for EOF ou EOL, para (statements opcionais)
            if (parser->current_token.type == TOKEN_EOF)
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

//==============================================================================
// statement       := assignment_stmt
//                  | input_stmt 
//                  | print_stmt
//                  | color_stmt 
//                  | expression_stmt
//==============================================================================
static ASTNode* parse_statement(Parser* parser)
{
    if (parser->current_token.type == TOKEN_LET)
    {
        return parse_assignment_stmt(parser);
    }
    else if(parser->current_token.type == TOKEN_INPUT)
    {
        return parse_input_statement(parser);
    } 
    else if (parser->current_token.type == TOKEN_PRINT || 
             parser->current_token.type == TOKEN_QUESTION)
    {
        return parse_print_statement(parser);  
    }
    else if (is_color_statement_token(parser->current_token.type))
    {
        // aqui parseia nocolor sozinho
        return parse_color_statement(parser);
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
// print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)
// print_item      := expression | 'width'(NUMBER)? |  ('left' | 'right' | 'center')? 
// nl              := 'nl'        # New line - quando presente, quebra linha
//===================================================================
static ASTNode* parse_print_statement(Parser* parser)
{
    // Salva posição do print/?
    int line = parser->current_token.line;
    int column = parser->current_token.column;
   
    parser_advance(parser); // Consome o token print ou ?
    
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
        

        // ============================================
        // CASOS ESPECIAIS PARA PRINT
        // ============================================
        
        // 1. CORES
        if (is_color_token(token.type))
        {
            ASTNode* color_node = create_color_node(token.type, 
                                                   token.line, 
                                                   token.column);
            if (!color_node)
            {
                parser_set_error(parser, "Error: could not create color node");
                free_ast(print_node);
                return NULL;
            }
            print_node_add_item(print_node, color_node);
            parser_advance(parser);  // Consome o token de cor
            continue;  // Continua para próximo item
        }
        // 2. WIDTH
        else if(token.type == TOKEN_WIDTH)
        {
            parser_advance(parser);  // Consome o 'width'
            if(parser->current_token.type != TOKEN_LPAREN)
            {
                parser_set_error(parser, "Error: '(' expected");
                free_ast(print_node);
                return NULL;
            }
            parser_advance(parser);  // Consome o '('

            // Valida se o usuario passou um numero como argumento do width()
            if(parser->current_token.type != TOKEN_NUMBER)
            {
                parser_set_error(parser, "Error: number expected in width()");
                free_ast(print_node);
                return NULL;
            }

            int width_value  = parser->current_token.value.number;

            // Valida o intervalo aceitável para width
            if (width_value < 0 || width_value > 256) {
                parser_set_error(parser, "Error: width must be between 1 and 256");
                free_ast(print_node);
                return NULL;
            }

            parser_advance(parser);  // Consome o numero
            if(parser->current_token.type != TOKEN_RPAREN)
            {
                parser_set_error(parser, "Error: ')' expected");
                free_ast(print_node);
                return NULL;
            }           
            ASTNode* width_node = create_width_node(width_value, token.line, token.column);
            print_node_add_item(print_node, width_node);
            parser_advance(parser);  // Consome o ')'
            continue;
        }
        // 3. ALIGNMENT
        else if(token.type == TOKEN_LEFT || token.type == TOKEN_RIGHT || token.type == TOKEN_CENTER)
        {
            TokenType token_type;

             switch(token.type)
             {
                case TOKEN_LEFT:
                    token_type = TOKEN_LEFT;break;
                case TOKEN_RIGHT:
                    token_type = TOKEN_RIGHT;break;
                case TOKEN_CENTER:
                    token_type = TOKEN_CENTER;break;
             }

            ASTNode* alignment_node = create_alignment_node(token_type, token.line, token.column);
            print_node_add_item(print_node, alignment_node);
            parser_advance(parser);  // Consome o 'left'|'right'|'center'
            continue;
        }

        // 3. EXPRESSÕES NORMAIS (números, strings, variáveis, etc)
        ASTNode* item = parse_expression(parser);
        if (parser->has_error)
        {
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
// color_stmt := COLOR_TOKEN (apenas nocolor por enquanto)
//===================================================================
static ASTNode* parse_color_statement(Parser* parser)
{
    Token token = parser->current_token;
    
    // Verifica se é um token de cor permitido como statement
    if (!is_color_statement_token(token.type))
    {
        char error_msg[BUFFER_SIZE];
        snprintf(error_msg, sizeof(error_msg),
            "Error [%d:%d]: Color '%s' cannot be used as standalone statement",
            token.line, token.column, token.text);
        parser_set_error(parser, error_msg);
        return NULL;
    }
    
    // Cria nó de cor
    ASTNode* node = create_color_node(token.type, token.line, token.column);
    
    if (!node) {
        parser_set_error(parser, "Error: could not create color node");
        return NULL;
    }
    
    parser_advance(parser);  // Consome o token
    
    return node;
}


//===================================================================
// input_stmt       := 'INPUT' (STRING)? IDENTIFIER  
//===================================================================
static ASTNode* parse_input_statement(Parser* parser)
{
    // Salva posição do input
    int line = parser->current_token.line;
    int column = parser->current_token.column;

    parser_advance(parser); // Consome o token input

    char prompt[STRING_SIZE] = {0};

    // Verifica se tem prompt 
    if (parser->current_token.type == TOKEN_STRING)
    {
        strncpy(prompt, parser->current_token.value.string, STRING_SIZE - 1);
        prompt[STRING_SIZE - 1] = '\0';
        
        parser_advance(parser);  // Consome string
    }

    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        parser_set_error(parser, "Error: Expected identifier after 'input' statment");
        return NULL;        
    }

    char var_name[VARNAME_SIZE];
    strncpy(var_name, parser->current_token.value.varname, VARNAME_SIZE - 1);
    var_name[VARNAME_SIZE - 1] = '\0';

    parser_advance(parser);  // Consome IDENTIFIER
    
    return create_input_node(prompt, var_name, line, column);
}

//===================================================================
// if_stmt
//===================================================================


//===================================================================
// expression_stmt := logical_expr
//===================================================================
static ASTNode* parse_expression_stmt(Parser* parser) {
    return parse_logical_expr(parser);
}


//===================================================================
// logical_expr := logical_or_expr
//===================================================================
static ASTNode* parse_logical_expr(Parser* parser) {
    return parse_logical_or_expr(parser);
}

//===================================================================
// logical_or_expr := logical_and_expr ('or' logical_and_expr)*
//===================================================================
//===================================================================
// logical_or_expr := logical_and_expr ('or' logical_and_expr)*
//===================================================================
static ASTNode* parse_logical_or_expr(Parser* parser)
{
    ASTNode* left = parse_logical_and_expr(parser);
    if (parser->has_error || !left) return NULL;
    
    while (parser->current_token.type == TOKEN_OR)
    {
        parser_advance(parser);// consome 'or'
        
        ASTNode* right = parse_logical_and_expr(parser);
        if (!right)
        {
            free_ast(left);
            return NULL;
        }
        
        left = create_logical_op_node(OP_OR, left, right, 
                                     parser->current_token.line, 
                                     parser->current_token.column);
    }
    
    return left;
}

//===================================================================
// logical_and_expr := not_expr ('and' not_expr)*
//===================================================================
static ASTNode* parse_logical_and_expr(Parser* parser)
{
    ASTNode* left = parse_not_expr(parser);
    if (parser->has_error || !left) return NULL;
    
    while (parser->current_token.type == TOKEN_AND) {
        parser_advance(parser);
        
        ASTNode* right = parse_not_expr(parser);
        if (!right)
        {
            free_ast(left);
            return NULL;
        }

        left = create_logical_op_node(OP_AND, left, right, 
                                     parser->current_token.line, 
                                     parser->current_token.column);
    }
    
    return left;
}

//===================================================================
// not_expr := ('not' | '!')? comparison_expr
//===================================================================
static ASTNode* parse_not_expr(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    // Verifica se tem 'not' ou '!'
    if (parser->current_token.type == TOKEN_NOT) 
    {
        parser_advance(parser);  // Consome 'not' ou '!'
        
        ASTNode* operand = parse_comparison_expr(parser);  // ← precisa de parse_comparison_expr
        if (!operand) return NULL;
        
        return create_logical_not_node(operand, line, column);
    }
    
    // Se não tem NOT, passa para comparação
    return parse_comparison_expr(parser);
}

//===================================================================
// comparison_expr := expression (comparison_op expression)*
//===================================================================
static ASTNode* parse_comparison_expr(Parser* parser)
{
    ASTNode* left = parse_expression(parser);  
    if (parser->has_error || !left) return NULL;
    
    // Verifica operadores relacionais
    while (1)
    {
        LogicalOperator op;
        int has_op = 0;
        
        // Captura posição do operador
        int op_line = parser->current_token.line;
        int op_column = parser->current_token.column;
        
        // Verifica cada operador relacional
        switch (parser->current_token.type)
        {
            case TOKEN_EQUAL:
                op = OP_EQUAL;
                has_op = 1;
                break;
            case TOKEN_NOT_EQUAL:
                op = OP_NOT_EQUAL;
                has_op = 1;
                break;
            case TOKEN_LESS:
                op = OP_LESS;
                has_op = 1;
                break;
            case TOKEN_GREATER:
                op = OP_GREATER;
                has_op = 1;
                break;
            case TOKEN_LESS_EQUAL:
                op = OP_LESS_EQUAL;
                has_op = 1;
                break;
            case TOKEN_GREATER_EQUAL:
                op = OP_GREATER_EQUAL;
                has_op = 1;
                break;
            default:
                has_op = 0;
                break;
        }
        
        if (!has_op)
        {
            // Não tem operador relacional, retorna o left
            return left;
        }
        
        // Consome o operador
        parser_advance(parser);
        
        // Parseia o lado direito
        ASTNode* right = parse_expression(parser);
        if (parser->has_error || !right)
        {
            free_ast(left);
            return NULL;
        }
        
        // Cria nó de comparação
        left = create_comparison_op_node(op,
                                      left, right,
                                      op_line, op_column);
    }
}

//===================================================================
// expression := term (('+' | '-') term)*
//===================================================================
static ASTNode* parse_expression(Parser* parser) {
    ASTNode* node = parse_term(parser);
    if (parser->has_error || !node) return NULL;
    
    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS)
    {
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
        if (parser->has_error || !right)
        {
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
    if (parser->has_error || !node) return NULL;
    
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
        if (parser->has_error || !right)
        {
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
    if (parser->has_error || !operand) return NULL;
    
    if (op == '+')
    {
        return operand;  
    }
    
    return create_unary_op_node(op, operand, line, column);
}


//===================================================================
// atom                := NUMBER 
//                     | STRING 
//                     | 'true' 
//                     | 'false' 
//                     | IDENTIFIER 
//                     | '(' logical_expr ')'
//===================================================================
static ASTNode* parse_atom(Parser* parser)
{
    Token token = parser->current_token;
    
    switch (token.type)
    {
        case TOKEN_TRUE:
            parser_advance(parser);
            return create_bool_node(1, token.line, token.column);

        case TOKEN_FALSE:
            parser_advance(parser);
            return create_bool_node(0, token.line, token.column);

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
            //parser_set_error(parser, "Error: Unexpected factor");
            report_unexpected_token_error(parser, "expression");
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

#ifdef TEST
#include "color.h"
#include "utils.h"

int main()
{
    setup_utf8();
    
    printf("%s=== TESTE PARSER v0.5.1 expressões de comparação e lógicas ===%s\n\n", 
           COLOR_HEADER, COLOR_RESET);
    
    char* testes[] =
    {

        // Expressões Simples
        "5 + 3",
        "x - y",
        "a * b / c",

        // Comparações
        "5 == 5",
        "x != 10",
        "a < b",
        "c > d",
        "e <= f",
        "g >= h",

        // Lógica Simples
        "true and false",
        "true or false",
        "not true",
        "! false",

        // Expressões Complexas
        "!(5 + 3) > 7 and (x < 10)",  
        "not (5 > 10) or (y == 20)",
        "(a > 0) and (b < 100) and (c != 50)",
        "x > 5 or y < 10 and z == 15"

    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    
    for (int i = 0; i < num_testes; i++)
    {
        printf("%s=== Teste %d: '%s' ===%s\n", 
               COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        ASTNode* ast = parse(&lexer);
        
        if (ast)
        {
            printf("AST gerada:\n");
            print_ast(ast, 0);
            free_ast(ast);
            printf("%sParsing OK%s\n", COLOR_SUCCESS, COLOR_RESET);
        }
        else
        {
            printf("%sERRO no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
        
        printf("\n");

        wait();
    }
    
    printf("\n%s=== TODOS OS TESTES COMPLETADOS ===%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    a89check_leaks();
    return 0;
}
#endif

// Fim de parser.c