// parser.c
/********************************************************************
OBSERVÇÕES:

* As funções de criação de nodes não testam se o node foi criado 
  com sucesso porque:
    * estas funções chamam create_node()
    * create_node() aloca memória através de A89ALLOC
    * A89ALLOC, se não conseguir alocar memória, chama exit() para
      encerrar o programa. 
********************************************************************/
#include "color.h"
#include "utils.h"
#include "ast.h"
#include "parser.h"
#include "a89alloc.h"

//===================================================================
// PROTÓTIPOS DAS FUNÇÕES 
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
static ASTNode* parse_stmt_list(Parser* parser);
static ASTNode* parse_stmt(Parser* parser);
static ASTNode* parse_assignment_stmt(Parser* parser);
static ASTNode* parse_print_stmt(Parser* parser);
static ASTNode* parse_color_stmt(Parser* parser);
static ASTNode* parse_input_stmt(Parser* parser);

static ASTNode* parse_if_stmt(Parser* parser);

static ASTNode* parse_while_stmt(Parser* parser);
static ASTNode* parse_break_stmt(Parser* parser);
static ASTNode* parse_continue_stmt(Parser* parser);

static ASTNode* parse_for_stmt(Parser* parser);

static ASTNode* parse_import_stmt(Parser* parser);

static ASTNode* parse_load_expr(Parser* parser);
static ASTNode* parse_save_stmt(Parser* parser);

static ASTNode* parse_function_call(Parser* parser, const char* function_name);

static ASTNode* parse_expr_stmt(Parser* parser);

// EXPRESSÕES LÓGICAS
static ASTNode* parse_logical_expr(Parser* parser);
static ASTNode* parse_logical_or_expr(Parser* parser);
static ASTNode* parse_logical_and_expr(Parser* parser);
static ASTNode* parse_not_expr(Parser* parser);
static ASTNode* parse_comparison_expr(Parser* parser);

// EXPRESSÕES ARITMÉTICAS
static ASTNode* parse_expr(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_postfix(Parser* parser);
static ASTNode* parse_atom(Parser* parser);

// ASTNode* parse(Lexer* lexer)
// ASTNode* parse_single_stmt(Lexer* lexer) 
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

static void parser_set_error(Parser* parser, const char* message)
{
    parser->has_error = 1;
    
    // Formata a mensagem de erro
    char error_text[BUFFER_SIZE];
    snprintf(error_text, sizeof(error_text),
             "[%d:%d] Parser error: %s",
             parser->current_token.line,
             parser->current_token.column,
             message);

    if(!has_lexical_error)
    {
        // Imprime cabeçalho de erro com linha:coluna
        fprintf(stderr, "%s[%d:%d] Parser error:%s %s\n",
                COLOR_ERROR, 
                parser->current_token.line,
                parser->current_token.column,
                COLOR_RESET, 
                message);

        display_error_location(parser->lexer->source,
                               parser->lexer->source_size,
                               parser->current_token.line,
                               parser->current_token.column);
    }
    
    // Copia para o buffer do parser
    strncpy(parser->error_message, error_text, sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}

// Verifica se um token é palavra-chave/comando
static int is_keyword_token(TokenType type)
{
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
static const char* get_keyword_name(TokenType type)
{
    switch (type)
    {
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
static int is_color_stmt_token(TokenType type) {
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
            "[%d:%d]: print statement cannot have ';' after it. "
            "Remove the ';' or use: print \"text1\" \"text2\" nl",
            token.line, token.column);
    }
    else if (token.type == TOKEN_COLON) {
        snprintf(error_msg, sizeof(error_msg),
            "[%d:%d]: print statement cannot have ':' after it. "
            "Use new line for next statement.",
            token.line, token.column);
    }
    else if (is_keyword_token(token.type)) {
        const char* keyword_name = get_keyword_name(token.type);
        
        // Mensagens específicas por tipo de comando
        if (token.type == TOKEN_LET) {
            snprintf(error_msg, sizeof(error_msg),
                "[%d:%d]: '%s' is a command, not a valid expression. "
                "Assign variables BEFORE printing:\n"
                "  let x = 5\n"
                "  print x nl",
                token.line, token.column, keyword_name);
        }
        else if (token.type == TOKEN_PRINT || token.type == TOKEN_QUESTION) {
            snprintf(error_msg, sizeof(error_msg),
                "[%d:%d]: '%s' is a command, not a valid expression. "
                "Use ONE print with multiple items:\n"
                "  print \"text1\" \"text2\" nl  (instead of print \"text1\" print \"text2\")",
                token.line, token.column, keyword_name);
        }
        else {
            // Mensagem genérica para outros comandos (futuro)
            snprintf(error_msg, sizeof(error_msg),
                "[%d:%d]: '%s' is a command, not a valid expression in print statement.",
                token.line, token.column, keyword_name);
        }
    }
    else {
        // Erro genérico para outros tokens inesperados
        snprintf(error_msg, sizeof(error_msg),
            "[%d:%d]: Unexpected '%s' in print statement",
            token.line, token.column, token.token_text[0] ? token.token_text : "token");
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
            "Operator '%s ' cannot appear at this position in %s",
            token.token_text, context);
    }
    else if (token.type == TOKEN_ERROR)
    {
        // Já tem mensagem de erro do lexer
        strncpy(error_msg, token.token_text, sizeof(error_msg) - 1);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg),
            "Unexpected '%s ' in %s (expected number, string, identifier or '(  ')",
            token.token_text, context);
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

    return parse_stmt_list(parser);
}

//===================================================================
// statement_list := statement (separator statement)*
// separator      := EOL | ':' | ';'
//
// Parseia uma lista de statements até encontrar (mas não consome):
// - TOKEN_END   (ex: antes de 'end while', 'end if', etc...)
// - TOKEN_ELSE  (ex: antes do 'else' em if-then-else) 
// - TOKEN_EOF   (fim de arquivo)
//
// Exige pelo menos um statement. Blocos vazios são proibidos.
//===================================================================
static ASTNode* parse_stmt_list(Parser* parser)
{
    ASTNode* list = create_statement_list_node(
        parser->current_token.line,
        parser->current_token.column
    );

    // Pula EOLs iniciais (linhas vazias ou com apenas comentários)
    while (parser->current_token.type == TOKEN_EOL)
    {
        parser_advance(parser);
    }
    
    // Verifica se há statements
    if (parser->current_token.type == TOKEN_EOF ||
        parser->current_token.type == TOKEN_END ||
        parser->current_token.type == TOKEN_ELSE)
    {
        // Lista vazia é permitida
        return list;
    }
        
    // Parseia o primeiro statement
    ASTNode* stmt = parse_stmt(parser);
    if (!stmt || parser->has_error) {
        free_ast(list);
        return NULL;
    }
    statement_list_add(list, stmt);
    
    // Continua parseando enquanto encontrar separadores
    while (!parser->has_error && 
           parser->current_token.type != TOKEN_EOF &&
           parser->current_token.type != TOKEN_END &&
           parser->current_token.type != TOKEN_ELSE)
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
                // Permite separadores no final 
                break;
            }

            if (parser->current_token.type == TOKEN_END ||
                parser->current_token.type == TOKEN_ELSE)
            {
                break;
            }

            // Parseia próximo statement
            stmt = parse_stmt(parser);
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
// statement           := assignment_stmt
//                     | print_stmt
//                     | color_stmt 
//                     | import_stmt
//                     | input_stmt 
//                     | if_stmt
//                     | while_stmt
//                     | for_stmt
//                     | break_stmt
//                     | continue_stmt
//                     | save_stmt
//                     | expression_stmt
//==============================================================================
static ASTNode* parse_stmt(Parser* parser)
{
    if (parser->current_token.type == TOKEN_LET)
    {
        return parse_assignment_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_PRINT || 
             parser->current_token.type == TOKEN_QUESTION)
    {
        return parse_print_stmt(parser);  
    }
    else if (is_color_stmt_token(parser->current_token.type))
    {
        // aqui parseia nocolor sozinho
        return parse_color_stmt(parser);
    }
    else if(parser->current_token.type == TOKEN_INPUT)
    {
        return parse_input_stmt(parser);
    } 
    else if (parser->current_token.type == TOKEN_IF)  
    {
        return parse_if_stmt(parser);            
    }
    else if (parser->current_token.type == TOKEN_WHILE)
    {
        return parse_while_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_FOR)
    {
        return parse_for_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_BREAK)
    {
        return parse_break_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_CONTINUE)
    {
        return parse_continue_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_IMPORT)
    {
        return parse_import_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_FROM)
    {
        return parse_import_stmt(parser);
    }
    else if (parser->current_token.type == TOKEN_SAVE)
    {
        return parse_save_stmt(parser);
    }
    else
    {
        return parse_expr_stmt(parser);
    }
}


//===================================================================
// assignment_stmt := LET (identifier | identifier '[' expr ']') '=' expression
//===================================================================
static ASTNode* parse_assignment_stmt(Parser* parser)
{
    // TOKEN_LET verificado antes da chamada a essa função
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
                "[%d:%d]: '%s' is a command keyword, cannot be used as variable name",
                parser->current_token.line, parser->current_token.column, keyword_name);
            parser_set_error(parser, error_msg);
        }
        else
        {
            parser_set_error(parser, "expected identifier");
        }
        return NULL;
    }

    // Cria nó de variável
    ASTNode* target = create_variable_node(parser->current_token.value.varname,
                                           parser->current_token.line,
                                           parser->current_token.column);
    if (!target) {
        parser_set_error(parser, "could not create variable node");
        return NULL;
    }

    parser_advance(parser);  // Consume identifier
    
    // Verifica se há '[' para array indexing
    while (parser->current_token.type == TOKEN_LBRACKET)
    {
        int line = parser->current_token.line;
        int column = parser->current_token.column;
        
        parser_advance(parser);  // Consume '['
        
        // Parse a expressão dentro dos colchetes
        ASTNode* index = parse_logical_expr(parser);
        if (parser->has_error || !index)
        {
            free_ast(target);
            return NULL;
        }
        
        // Verifica se há ']'
        if (parser->current_token.type != TOKEN_RBRACKET)
        {
            parser_set_error(parser, "expected ']' after array index");
            free_ast(target);
            free_ast(index);
            return NULL;
        }
        
        parser_advance(parser);  // Consume ']'
        
        // Cria nó de array access
        target = create_array_access_node(target, index, line, column);
        if (!target)
        {
            parser_set_error(parser, "could not create array access node");
            return NULL;
        }
    }
    
    // Check '='
    if (parser->current_token.type != TOKEN_ASSIGN) {
        parser_set_error(parser, "expected '=' after variable name");
        free_ast(target);
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

        return create_assignment_node(target, string_node,
                                      string_node->line,
                                      string_node->column);
    }

    ASTNode* expr = parse_expr(parser);
    if (parser->has_error) {
        free_ast(target);
        return NULL;
    }

    return create_assignment_node(target, expr, expr->line, expr->column);
}


//===================================================================
// print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)
// print_item      := expression | 'width'(NUMBER)? |  ('left' | 'right' | 'center')? 
// nl              := 'nl'        # New line - quando presente, quebra linha
//===================================================================
static ASTNode* parse_print_stmt(Parser* parser)
{
    // Salva posição do print/?
    int line = parser->current_token.line;
    int column = parser->current_token.column;
   
    parser_advance(parser); // Consome o token print ou ?
    
    // Cria nó do comando print
    ASTNode* print_node = create_print_node(line, column);
    
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
                parser_set_error(parser,
                    "'(' expected after 'width'."
                    " 'width' parameter requires value in parentheses."
                    " Example: width(10)");
                free_ast(print_node);
                return NULL;
            }
            parser_advance(parser);  // Consome o '('

            // Valida se o usuario passou um numero como argumento do width()
            if(parser->current_token.type != TOKEN_NUMBER)
            {
                parser_set_error(parser,
                    "number expected in width()"
                    " Example: width(10)");
                free_ast(print_node);
                return NULL;
            }

            int width_value  = parser->current_token.value.number;

            // Valida o intervalo aceitável para width
            if (width_value < 0 || width_value > 256) {
                parser_set_error(parser, "width must be between 1 and 256");
                free_ast(print_node);
                return NULL;
            }

            parser_advance(parser);  // Consome o numero
            if(parser->current_token.type != TOKEN_RPAREN)
            {
                parser_set_error(parser,
                    "')' expected."
                    " 'width' parameter requires value in parentheses."
                    " Example: width(10)");
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
        //ASTNode* item = parse_expr(parser);
        ASTNode* item = parse_logical_expr(parser);
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
static ASTNode* parse_color_stmt(Parser* parser)
{
    Token token = parser->current_token;
    
    // Verifica se é um token de cor permitido como statement
    if (!is_color_stmt_token(token.type))
    {
        char error_msg[BUFFER_SIZE];
        snprintf(error_msg, sizeof(error_msg),
            "[%d:%d]: Color '%s' cannot be used as standalone statement",
            token.line, token.column, token.token_text);
        parser_set_error(parser, error_msg);
        return NULL;
    }
    
    // Cria nó de cor
    ASTNode* node = create_color_node(token.type, token.line, token.column);

    parser_advance(parser);  // Consome o token
    
    return node;
}


//===================================================================
// input_stmt := 'input' (color_directive)? (width_spec)? (alignment)? (STRING)? (nocolor)? IDENTIFIER
//===================================================================
static ASTNode* parse_input_stmt(Parser* parser)
{
    // Salva posição do input
    int line = parser->current_token.line;
    int column = parser->current_token.column;

    parser_advance(parser); // Consome o token input

    ASTNode* color_node = NULL;
    ASTNode* width_node = NULL;
    ASTNode* alignment_node = NULL;

    // PROCESSA COLOR
    if (is_color_token(parser->current_token.type))
    {
        color_node = create_color_node(parser->current_token.type, 
                                       parser->current_token.line,
                                       parser->current_token.column);
        parser_advance(parser);  // Consome o token de cor
    }
    
    // PROCESSA WIDTH
    if(parser->current_token.type == TOKEN_WIDTH)
    {
        parser_advance(parser);  // Consome o 'width'
        if(parser->current_token.type != TOKEN_LPAREN)
        {
            parser_set_error(parser,
                "'(' expected after 'width'."
                " 'width' parameter requires value in parentheses."
                " Example: width(10)");
            free_ast(color_node);
            return NULL;
        }
        parser_advance(parser);  // Consome o '('

        // Valida se o usuario passou um numero como argumento do width()
        if(parser->current_token.type != TOKEN_NUMBER)
        {
            parser_set_error(parser,
                "number expected in width()"
                " Example: width(10)");
            free_ast(color_node);
            return NULL;
        }

        int width_value  = parser->current_token.value.number;

        // Valida o intervalo aceitável para width
        if (width_value < 0 || width_value > 256) {
            parser_set_error(parser, "width must be between 1 and 256");
            free_ast(color_node);
            return NULL;
        }

        parser_advance(parser);  // Consome o numero
        if(parser->current_token.type != TOKEN_RPAREN)
        {
            parser_set_error(parser,
                "')' expected."
                " 'width' parameter requires value in parentheses."
                " Example: width(10)");
            free_ast(color_node);
            return NULL;
        }           
        
        width_node = create_width_node(width_value,
                                      parser->current_token.line,
                                      parser->current_token.column);
        parser_advance(parser);  // Consome o ')'
    }

    // PROCESSA O ALIGNMENT
    if(parser->current_token.type == TOKEN_LEFT ||
       parser->current_token.type == TOKEN_RIGHT ||
       parser->current_token.type == TOKEN_CENTER)
    {
        TokenType token_type;

         switch(parser->current_token.type)
         {
            case TOKEN_LEFT:
                token_type = TOKEN_LEFT;break;
            case TOKEN_RIGHT:
                token_type = TOKEN_RIGHT;break;
            case TOKEN_CENTER:
                token_type = TOKEN_CENTER;break;
         }

        alignment_node = create_alignment_node(token_type,
                                               parser->current_token.line,
                                               parser->current_token.column);
        parser_advance(parser);  // Consome o 'left'|'right'|'center'
    }

    // PROCESSA O PROMPT
    char prompt[STRING_SIZE] = {0};

    // Verifica se tem prompt 
    if (parser->current_token.type == TOKEN_STRING)
    {
        strncpy(prompt, parser->current_token.value.string, STRING_SIZE - 1);
        prompt[STRING_SIZE - 1] = '\0';
        
        parser_advance(parser);  // Consome string
    }

    // PROCESSA O NOCOLOR
    int set_nocolor = 0;
    if (parser->current_token.type == TOKEN_NOCOLOR)
    {
        set_nocolor = 1;
        parser_advance(parser);  // Consome nocolor

    }  

    // PROCESSA IDENTIFIER
    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        parser_set_error(parser, "expected identifier after 'input' statment");
        if(color_node) free_ast(color_node);
        if(width_node) free_ast(width_node);
        if(alignment_node) free_ast(alignment_node);
        return NULL;        
    }

    char var_name[VARNAME_SIZE];
    strncpy(var_name, parser->current_token.value.varname, VARNAME_SIZE - 1);
    var_name[VARNAME_SIZE - 1] = '\0';

    parser_advance(parser);  // Consome IDENTIFIER
    
    return create_input_node(color_node, width_node, alignment_node,
                             prompt,
                             set_nocolor,
                             var_name,
                             line, column);
}

//===================================================================
// if_stmt := 'if' logical_expr 'then' EOL
//                 statement_list
//             ('else' 'if' logical_expr 'then' EOL
//                 statement_list )*
//             ('else' EOL
//                 statement_list )?
//            'end' 'if' EOL
//===================================================================
static ASTNode* parse_if_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    parser_advance(parser);// Consome 'if'
    
    if (parser->current_token.type != TOKEN_LPAREN)
    {
        parser_set_error(parser, "'(' expected after 'if'");
        return NULL;
    }
    parser_advance(parser);// Consome '('
    
    ASTNode* condition = parse_logical_expr(parser);
    if (parser->has_error || !condition)
    {
        return NULL;
    }
    
    if (parser->current_token.type != TOKEN_RPAREN)
    {
        parser_set_error(parser, "')' expected after condition in 'if' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);// Consome ')'
    
    if (parser->current_token.type != TOKEN_THEN)
    {
        parser_set_error(parser, "'then' expected after condition in 'if' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);// Consome 'then'
    
    // Espera EOL/NL
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL)
    {
        parser_set_error(parser, "newline expected after 'then' in 'if' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);// Consome EOL/NL
    
    // Parse then body (statement list)
    ASTNode* then_body = parse_stmt_list(parser);
    if (parser->has_error || !then_body)
    {
        free_ast(condition);
        return NULL;
    }
    
    // Processa múltiplos 'else if' (0 ou mais)
    // ( 'else' 'if' logical_expr 'then' EOL statement_list )*
    ASTNode* else_body = NULL;
    while (parser->current_token.type == TOKEN_ELSE)
    {
        parser_advance(parser);// Consome 'else'
        
        // Verifica se é 'else if' ou 'else' final
        if (parser->current_token.type == TOKEN_IF)
        {
            // É 'else if' - processa como novo IF sem 'if' keyword
            parser_advance(parser);  // Consome 'if'
            
            if (parser->current_token.type != TOKEN_LPAREN)
            {
                parser_set_error(parser, "'(' expected after 'if'");
                free_ast(condition);
                free_ast(then_body);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            parser_advance(parser);  // Consome '('
            
            ASTNode* elif_condition = parse_logical_expr(parser);
            if (parser->has_error || !elif_condition)
            {
                free_ast(condition);
                free_ast(then_body);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            
            if (parser->current_token.type != TOKEN_RPAREN)
            {
                parser_set_error(parser, "')' expected after condition in 'if' statement");
                free_ast(condition);
                free_ast(then_body);
                free_ast(elif_condition);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            parser_advance(parser);  // Consome ')'
            
            if (parser->current_token.type != TOKEN_THEN)
            {
                parser_set_error(parser, "'then' expected after condition in 'if' statement");
                free_ast(condition);
                free_ast(then_body);
                free_ast(elif_condition);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            parser_advance(parser);  // Consome 'then'
            
            // Espera EOL/NL
            if (parser->current_token.type != TOKEN_EOL && 
                parser->current_token.type != TOKEN_NL)
            {
                parser_set_error(parser, "newline expected after 'then' in 'if' statement");
                free_ast(condition);
                free_ast(then_body);
                free_ast(elif_condition);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            parser_advance(parser);  // Consome EOL/NL
            
            // Parse elif body (statement list)
            ASTNode* elif_body = parse_stmt_list(parser);
            if (parser->has_error || !elif_body)
            {
                free_ast(condition);
                free_ast(then_body);
                free_ast(elif_condition);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            
            // Cria um IF node para este 'else if'
            // Este IF será o else_body do IF anterior
            ASTNode* elif_node = create_if_node(elif_condition, elif_body, NULL, 
                                                elif_condition->line, elif_condition->column);
            
            // Encadeia: se else_body já existe, coloca elif_node como else_body dele
            if (else_body)
            {
                // else_body é um IF node anterior
                // Coloca elif_node como seu else_body
                else_body->data.if_stmt.else_body = elif_node;
            }
            else
            {
                // Primeiro elif
                else_body = elif_node;
            }
        }
        else
        {
            // É 'else' final (não é 'else if')
            // ( 'else' EOL statement_list )?
            
            // Espera EOL/NL
            if (parser->current_token.type != TOKEN_EOL && 
                parser->current_token.type != TOKEN_NL)
            {
                parser_set_error(parser, "newline expected after 'else' in 'if' statement");
                free_ast(condition);
                free_ast(then_body);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            parser_advance(parser);  // Consome EOL/NL
            
            // Parse else body (statement list)
            ASTNode* final_else_body = parse_stmt_list(parser);
            if (parser->has_error || !final_else_body)
            {
                free_ast(condition);
                free_ast(then_body);
                if (else_body) free_ast(else_body);
                return NULL;
            }
            
            // Coloca final_else_body como else_body do último elif
            if (else_body)
            {
                // Encontra o último elif e coloca final_else_body nele
                ASTNode* current = else_body;
                while (current->data.if_stmt.else_body != NULL)
                {
                    current = current->data.if_stmt.else_body;
                }
                current->data.if_stmt.else_body = final_else_body;
            }
            else
            {
                // Não há elif, else_body é o final_else_body
                else_body = final_else_body;
            }
            
            // Sai do loop (else final encontrado)
            break;
        }
    }
    
    // Verifica 'end if' (obrigatório)
    if (parser->current_token.type != TOKEN_END)
    {
        parser_set_error(parser, "'end' expected in 'if' statement");
        free_ast(condition);
        free_ast(then_body);
        if (else_body) free_ast(else_body);
        return NULL;
    }
    parser_advance(parser);  // Consome 'end'
    
    // Espera 'if'
    if (parser->current_token.type != TOKEN_IF)
    {
        parser_set_error(parser, "'if' expected after 'end'");
        free_ast(condition);
        free_ast(then_body);
        if (else_body) free_ast(else_body);
        return NULL;
    }
    parser_advance(parser);  // Consome 'if'
    
    // Expect EOL/NL (ou EOF)
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL &&
        parser->current_token.type != TOKEN_EOF)
    {
        parser_set_error(parser, "newline expected after 'end if'");
        free_ast(condition);
        free_ast(then_body);
        if (else_body) free_ast(else_body);
        return NULL;
    }
    
    // Consome EOL/NL se existir
    // if (parser->current_token.type == TOKEN_EOL || 
    //     parser->current_token.type == TOKEN_NL)
    // {
    //     parser_advance(parser);  // Consome EOL/NL
    // }
    
    // Create IF node
    ASTNode* if_node = create_if_node(condition, then_body, else_body, line, column);
    
    return if_node;
}

//===================================================================
// while_stmt := 'while' '(' logical_expr ')' 'do' EOL
//                  statement_list*
//               'end' 'while' EOL
//===================================================================
static ASTNode* parse_while_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    parser_advance(parser);  // Consome 'while'
    
    if (parser->current_token.type != TOKEN_LPAREN)
    {
        parser_set_error(parser, "'(' expected after 'while' statement");
        return NULL;
    }
    parser_advance(parser);  // Consome '('
    
    ASTNode* condition = parse_logical_expr(parser);
    if (parser->has_error || !condition)
    {
        return NULL;
    }
    
    if (parser->current_token.type != TOKEN_RPAREN)
    {
        parser_set_error(parser, "')' expected after condition in 'while' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);  // Consome ')'
    
    if (parser->current_token.type != TOKEN_DO)
    {
        parser_set_error(parser, "'do' expected after condition in 'while' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);  // Consome 'do'
    
    // Espera EOL/NL
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL)
    {
        parser_set_error(parser, "newline expected after 'do' in 'while' statement");
        free_ast(condition);
        return NULL;
    }
    parser_advance(parser);  // Consome EOL/NL
    
    // Parse body (statement list)
    ASTNode* body = parse_stmt_list(parser);
    if (parser->has_error || !body)
    {
        free_ast(condition);
        return NULL;
    }
    

    // Expect 'end' 'while'
    if (parser->current_token.type != TOKEN_END)
    {
        parser_set_error(parser, "'end' expected in 'while' statement");
        free_ast(condition);
        free_ast(body);
        return NULL;
    }
    parser_advance(parser);  // Consome 'end'
    
    // Expect 'while'
    if (parser->current_token.type != TOKEN_WHILE)
    {
        parser_set_error(parser, "'while' expected after 'end' in 'while' statement");
        free_ast(condition);
        free_ast(body);
        return NULL;
    }
    parser_advance(parser);  // Consome 'while'
    
    // Expect EOL/NL
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL &&
        parser->current_token.type != TOKEN_EOF)
    {
        parser_set_error(parser, "newline expected after 'end while'");
        free_ast(condition);
        free_ast(body);
        return NULL;
    }
    
    // if (parser->current_token.type == TOKEN_EOL || 
    //     parser->current_token.type == TOKEN_NL)
    // {
    //     parser_advance(parser);  // Consome EOL/NL
    // }
    
    // Create WHILE node
    ASTNode* while_node = create_while_node(condition, body, line, column);
    
    return while_node;
}

//===================================================================
// break_stmt := 'break' EOL
//===================================================================
static ASTNode* parse_break_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    parser_advance(parser);  // Consome 'break'
    
    // Expect EOL/NL
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL &&
        parser->current_token.type != TOKEN_EOF)
    {
        parser_set_error(parser, "Error: newline expected after 'break'");
        return NULL;
    }
    
    if (parser->current_token.type == TOKEN_EOL || 
        parser->current_token.type == TOKEN_NL)
    {
        parser_advance(parser);  // Consome EOL/NL
    }
    
    // Create BREAK node
    ASTNode* break_node = create_break_node(line, column);
    
    return break_node;
}

//===================================================================
// continue_stmt := 'continue' EOL
//===================================================================
static ASTNode* parse_continue_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    parser_advance(parser);  // Consome 'continue'
    
    // Expect EOL/NL
    if (parser->current_token.type != TOKEN_EOL && 
        parser->current_token.type != TOKEN_NL &&
        parser->current_token.type != TOKEN_EOF)
    {
        parser_set_error(parser, "newline expected after 'continue'");
        return NULL;
    }
    
    if (parser->current_token.type == TOKEN_EOL || 
        parser->current_token.type == TOKEN_NL)
    {
        parser_advance(parser);  // Consome EOL/NL
    }
    
    // Create CONTINUE node
    ASTNode* continue_node = create_continue_node(line, column);

    return continue_node;
}


//===================================================================
// for_stmt := 'for' IDENTIFIER '=' expression 'to' expression ('step' expression)? 'do' EOL
//                 statement_list
//             'end' 'for'
//===================================================================
static ASTNode* parse_for_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;

    parser_advance(parser); // Consome 'for'

    if (!parser_expect(parser, TOKEN_IDENTIFIER))
    {
        parser_set_error(parser, "expected identifier after 'for'");
        return NULL;
    }
    char var_name[VARNAME_SIZE];
    strncpy(var_name, parser->current_token.value.varname, VARNAME_SIZE - 1);
    var_name[VARNAME_SIZE - 1] = '\0';
    parser_advance(parser);

    if (!parser_expect(parser, TOKEN_ASSIGN))
    {
        parser_set_error(parser, "expected '=' after identifier in for loop");
        return NULL;
    }
    parser_advance(parser);

    ASTNode* init_value = parse_expr(parser);
    if (!init_value)
    {
        return NULL; 
    }

    if (!parser_expect(parser, TOKEN_TO))
    {
        parser_set_error(parser, "expected 'to' after initial value in for loop");
        free_ast(init_value);
        return NULL;
    }
    parser_advance(parser);

    ASTNode* end_value = parse_expr(parser);
    if (!end_value)
    {
        free_ast(init_value);
        return NULL; 
    }

    ASTNode* step_value = NULL;
    if (parser_expect(parser, TOKEN_STEP))
    {
        parser_advance(parser);
        step_value = parse_expr(parser);
        if (!step_value)
        {
            free_ast(init_value);
            free_ast(end_value);
            return NULL; 
        }
    }

    if (!parser_expect(parser, TOKEN_DO))
    {
        parser_set_error(parser, "expected 'do' after for loop conditions");
        free_ast(init_value);
        free_ast(end_value);
        if (step_value) free_ast(step_value);
        return NULL;
    }
    parser_advance(parser);

    if (!parser_expect(parser, TOKEN_EOL))
    {
        parser_set_error(parser, "expected EOL after 'do' in for loop");
        free_ast(init_value);
        free_ast(end_value);
        if (step_value) free_ast(step_value);
        return NULL;
    }
    parser_advance(parser);

    ASTNode* body = parse_stmt_list(parser);
    if (!body)
    {
        free_ast(init_value);
        free_ast(end_value);
        if (step_value) free_ast(step_value);
        return NULL; 
    }

    if (!parser_expect(parser, TOKEN_END)) {
        parser_set_error(parser, "expected 'end' to close for loop");
        free_ast(init_value);
        free_ast(end_value);
        if (step_value) free_ast(step_value);
        free_ast(body);
        return NULL;
    }
    parser_advance(parser);

    if (!parser_expect(parser, TOKEN_FOR))
    {
        parser_set_error(parser, "expected 'for' after 'end' to close for loop");
        free_ast(init_value);
        free_ast(end_value);
        if (step_value) free_ast(step_value);
        free_ast(body);
        return NULL;
    }
    parser_advance(parser);

    return create_for_node(var_name, init_value, end_value, step_value, body, line, column);
}

//===================================================================
// import_stmt := 'import' IDENTIFIER
//             | 'from' IDENTIFIER 'import' identifier_list
//
// identifier_list := IDENTIFIER (',' IDENTIFIER)*
//
// import math
// from math import sqrt, abs
//===================================================================
static ASTNode* parse_import_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    parser_advance(parser);  // Consume IMPORT ou FROM
    
    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        parser_set_error(parser, "expected module name after 'import' or 'from'");
        return NULL;
    }
    
    char module_name[VARNAME_SIZE];

    strcpy(module_name, parser->current_token.value.varname);

    parser_advance(parser);  // Consume module name
    
    // Verificar se é "import math" ou "from math import ..."
    int import_all = 1;
    char** imported_names = NULL;
    int imported_count = 0;
    
    // Alocar array para nomes importados
    imported_names = (char**)A89ALLOC(sizeof(char*) * 100);  // Max 100 funções

    if (parser->current_token.type == TOKEN_IMPORT)
    {
        // from math import sqrt, abs
        import_all = 0;
        parser_advance(parser);  // Consume IMPORT

        while (parser->current_token.type == TOKEN_IDENTIFIER)
        {
            imported_names[imported_count] = (char*)A89ALLOC(VARNAME_SIZE);

            strncpy(imported_names[imported_count], parser->current_token.value.varname, 
                    VARNAME_SIZE - 1);
            imported_names[imported_count][VARNAME_SIZE - 1] = '\0';

            imported_count++;
            parser_advance(parser);
            
            if (parser->current_token.type == TOKEN_COMMA)
            {
                parser_advance(parser);  // Consume COMMA
            }
            else
            {
                break;
            }
        }
    }
    
    return create_import_node(module_name, imported_names, imported_count, import_all, 
                             line, column);
}


//===================================================================
// load_expr := 'load' '(' STRING ')'
//
// let texto = load("arquivo.txt")
//===================================================================
static ASTNode* parse_load_expr(Parser* parser)
{
    // Ao chegar aqui 'load' já foi consumido

    int line = parser->current_token.line;
    int column = parser->current_token.column;

    if (parser->current_token.type != TOKEN_LPAREN)
    {
        parser_set_error(parser, "expected '(' after 'load'");
        return NULL;
    }
    parser_advance(parser);  // Consume (
    
    if (parser->current_token.type != TOKEN_STRING)
    {
        parser_set_error(parser, "expected filename string in load()");
        return NULL;
    }
    
    char filename[BUFFER_SIZE];
    strncpy(filename, parser->current_token.value.string, BUFFER_SIZE - 1);
    filename[BUFFER_SIZE - 1] = '\0';
    parser_advance(parser);  // Consume STRING
    
    if (parser->current_token.type != TOKEN_RPAREN)
    {
        parser_set_error(parser, "expected ')' after filename in load()");
        return NULL;
    }
    parser_advance(parser);  // Consume )
    
    return create_load_node(filename, line, column);
}

//===================================================================
// save_stmt := 'save' '(' expression ',' STRING ')'
//
// save(expressao, "arquivo.txt")
//===================================================================
static ASTNode* parse_save_stmt(Parser* parser)
{
    parser_advance(parser);  // Consume 'save'

    int line = parser->current_token.line;
    int column = parser->current_token.column;

    if (parser->current_token.type != TOKEN_LPAREN)
    {
        parser_set_error(parser, "expected '(' after 'save'");
        return NULL;
    }
    parser_advance(parser);  // Consume (
    
    // Parseia a expressão do Text
    ASTNode* expr = parse_expr_stmt(parser); 
    if (!expr)
    {
        parser_set_error(parser, "expected expression in save()");
        return NULL;
    }

    if (parser->current_token.type != TOKEN_COMMA)
    {
        parser_set_error(parser, "expected ',' in save()");
        free_ast(expr);
        return NULL;
    }
    parser_advance(parser);  // Consome ,
    
    if (parser->current_token.type != TOKEN_STRING)
    {
        parser_set_error(parser, "expected filename string in save()");
        free_ast(expr);
        return NULL;
    }
    
    char filename[BUFFER_SIZE];
    strncpy(filename, parser->current_token.value.string, BUFFER_SIZE - 1);
    filename[BUFFER_SIZE - 1] = '\0';
    parser_advance(parser);  // Consume STRING
    
    if (parser->current_token.type != TOKEN_RPAREN)
    {
        parser_set_error(parser, "expected ')' after filename in save()");
        free_ast(expr);
        return NULL;
    }
    parser_advance(parser);  // Consume )
    
    return create_save_node(expr, filename, line, column);
}

static ASTNode* parse_function_call(Parser* parser, const char* function_name)
{
    int line = parser->current_token.line;
    int col = parser->current_token.column;
    
    ASTNode* function_node = create_function_call_node(function_name, line, col);
    
    parser_advance(parser);  // Consume '('
    
    // Parse argumentos
    if (parser->current_token.type != TOKEN_RPAREN)
    {
        while (1)
        {
            ASTNode* arg = parse_logical_expr(parser);
            if (parser->has_error)
            {
                free_ast(function_node);
                return NULL;
            }
            
            function_call_add_argument(function_node, arg);
            
            if (parser->current_token.type == TOKEN_RPAREN)
                break;
            
            if (parser->current_token.type != TOKEN_COMMA)
            {
                parser_set_error(parser, "expected ',' or ')' in function call");
                free_ast(function_node);
                return NULL;
            }
            
            parser_advance(parser);  // Consume ','
        }
    }
    
    if (!parser_expect(parser, TOKEN_RPAREN))
    {
        free_ast(function_node);
        parser_set_error(parser, "expected ')' in function call");
        return NULL;
    }
    
    parser_advance(parser);  // Consume ')'
    
    return function_node;
}


//===================================================================
// expression_stmt := logical_expr
//===================================================================
static ASTNode* parse_expr_stmt(Parser* parser)
{
    return parse_logical_expr(parser);
}

//===================================================================
// logical_expr := logical_or_expr
//===================================================================
static ASTNode* parse_logical_expr(Parser* parser)
{
    return parse_logical_or_expr(parser);
}

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
    ASTNode* left = parse_expr(parser);  
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
        ASTNode* right = parse_expr(parser);
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
static ASTNode* parse_expr(Parser* parser) {
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
// factor := ('+' | '-')? postfix
//===================================================================
static ASTNode* parse_factor(Parser* parser)
{
    char op = '+';
    int line = parser->current_token.line;
    int column = parser->current_token.column;
    
    if (parser->current_token.type == TOKEN_PLUS || 
        parser->current_token.type == TOKEN_MINUS)
    {
        
        if (parser->current_token.type == TOKEN_MINUS)
        {
            op = '-';
        }
        parser_advance(parser);
    }
    
    ASTNode* operand = parse_postfix(parser);
    if (parser->has_error || !operand) return NULL;
    
    if (op == '+')
    {
        return operand;  
    }
    
    return create_unary_op_node(op, operand, line, column);
}

//===================================================================
// postfix := atom ('[' logical_expr ']')*
//===================================================================
static ASTNode* parse_postfix(Parser* parser)
{
    int line = parser->current_token.line;
    int column = parser->current_token.column;

    // Primeiro, parse o atom
    ASTNode* node = parse_atom(parser);
    if (parser->has_error || !node) return NULL;
    
    // Depois, processa quantos '[...]' houver
    while (parser->current_token.type == TOKEN_LBRACKET)
    {
        parser_advance(parser);  // Consume '['
        
        // Parse a expressão dentro dos colchetes
        ASTNode* index = parse_logical_expr(parser);
        if (parser->has_error || !index)
        {
            free_ast(node);
            return NULL;
        }
        
        // Verifica se há ']'
        if (parser->current_token.type != TOKEN_RBRACKET)
        {
            parser_set_error(parser, "expected ']' after array index");
            free_ast(node);
            free_ast(index);
            return NULL;
        }
        
        parser_advance(parser);  // Consume ']'
        
        // Cria nó de array access
        node = create_array_access_node(node, index, line, column);
        if (!node)
        {
            parser_set_error(parser, "could not create array access node");
            return NULL;
        }
    }
    
    return node;
}


//===================================================================
// atom                := NUMBER 
//                     | STRING 
//                     | TEXT
//                     | ARRAY
//                     | 'true' 
//                     | 'false' 
//                     | function_call
//                     | IDENTIFIER 
//                     | load_expr
//                     | '(' logical_expr ')'
//===================================================================
static ASTNode* parse_atom(Parser* parser)
{
    Token token = parser->current_token;
    
    switch (token.type)
    {
        case TOKEN_TRUE:
            parser_advance(parser);
            return create_bool_node(1,
                                    parser->current_token.line,
                                    parser->current_token.column);

        case TOKEN_FALSE:
            parser_advance(parser);
            return create_bool_node(0,
                                    parser->current_token.line,
                                    parser->current_token.column);

        case TOKEN_NUMBER:
            parser_advance(parser);
            return create_number_node(token.value.number,
                                      parser->current_token.line,
                                      parser->current_token.column);

        case TOKEN_STRING:
            parser_advance(parser);
            return create_string_node(token.value.string,
                                      parser->current_token.line,
                                      parser->current_token.column);

        case TOKEN_IDENTIFIER:
        {
            char name[BUFFER_SIZE];
            strncpy(name, token.value.varname, BUFFER_SIZE - 1);
            name[BUFFER_SIZE - 1] = '\0';
            int line   = token.line;
            int column = token.column;
            parser_advance(parser);
            
            // Verifica se é uma chamada de função
            if (parser->current_token.type == TOKEN_LPAREN)
            {
                return parse_function_call(parser, name);
            }
            
            // Senão, é uma variável
            return create_variable_node(name, line, column);
        }

        case TOKEN_LOAD:
            parser_advance(parser);
            return  parse_load_expr(parser);
            
        case TOKEN_LPAREN:
        {
            parser_advance(parser);  // Consume '('
            ASTNode* node = parse_logical_expr(parser);
            if (parser->has_error) return NULL;
            
            if (!parser_expect(parser, TOKEN_RPAREN))
            {
                free_ast(node);
                parser_set_error(parser, "expected ')'");
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
        //printf("%sParser error: incomplete expression.%s\n", COLOR_ERROR, COLOR_RESET);
        return NULL;
    }

    return result;
}

//===================================================================
// FOR TESTING V0.2.0
//===================================================================
ASTNode* parse_single_stmt(Lexer* lexer)
{
    Parser parser;
    parser_init(&parser, lexer);
    
    if (parser.current_token.type == TOKEN_EOF)
    {
        return NULL;
    }
    
    // Parse ONLY ONE statement (can be assignment OR expression)
    ASTNode* result = parse_stmt(&parser);
    
    if (parser.has_error)
    {
        if (result) free_ast(result);
        printf("%s\n", parser.error_message);
        return NULL;
    }

    // Para statements de bloco (while, if, break, continue),
    // consome o EOL após eles se existir
    if (result && (result->type == NODE_WHILE || 
                   result->type == NODE_IF ||
                   result->type == NODE_BREAK ||
                   result->type == NODE_CONTINUE))
    {
        while(parser.current_token.type == TOKEN_EOL ||
              parser.current_token.type == TOKEN_NL)
        { 
            parser_advance(&parser);
        }
    }
        
    return result;
}

#ifdef TESTPARSER
#include "color.h"
#include "utils.h"

int main()
{
    setup_utf8();
    
    printf("%s=== TESTE PARSER v0.6.0 ===%s\n\n", 
           COLOR_HEADER, COLOR_RESET);
    
    char* testes[] =
    {
        "let programa = load(\"calculadora.zz\")",

        "save(programa, \"backup.zz\")\n"
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
            //printf("%sERRO no parsing%s\n", COLOR_ERROR, COLOR_RESET);
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