// evaluator.c

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>  

#include "color.h"
#include "a89alloc.h"
#include "evaluator.h"

#define EPSILON 1e-12

// Cor atual sendo aplicada (estado global para sessão)
static const char* current_color_global = "";
static int colors_enabled_global = 1;


static EvaluatorResult create_success_result_bool(int value, int line, int column);
static EvaluatorResult create_success_result_number(double value, int line, int column);
static EvaluatorResult create_success_result_string(const char* value, int line, int column);
static EvaluatorResult create_error_result(const char* message, int line, int column);
static EvaluatorResult create_error_result_fmt(int line, int column, 
                                              const char* format, ...);

static void reset_current_color(void);
static void apply_color(const char* ansi_color);
static void detect_color_support(void);

static int count_utf8_chars(const char* str);
static AlignmentType token_to_alignment(TokenType token_type);
static void reset_format(ExecutionContext* ctx);
static void update_format_from_node(ASTNode* node, ExecutionContext* ctx);
static void apply_format(const char* str, OutputFormat* format);
void evaluator_reset_format(ExecutionContext* ctx);

ExecutionContext* execution_ctx_create(SymbolTable* symbols);
void execution_ctx_destroy(ExecutionContext* ctx);

static int is_numeric_string(const char* str);
static char* read_user_input(const char* prompt);

static int evaluate_input_stmt(ASTNode* node, SymbolTable* symbols);

static int execute_stmt_list(ASTNode* node, SymbolTable* symbols);
static int execute_stmt(ASTNode* node, SymbolTable* symbols);
static int execute_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

static int evaluate_print_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int evaluate_print_stmt(ASTNode* node, SymbolTable* symbols);
static int evaluate_print_stmt_with_format(ASTNode* node, ExecutionContext* ctx);

static int execute_if_stmt(ASTNode* node, SymbolTable* symbols);
static int execute_if_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

static int execute_while_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

// FUNÇÕES PÚBLICAS
// int evaluate_program(ASTNode* node, SymbolTable* symbols);
// EvaluatorResult evaluate_expr(ASTNode* node, SymbolTable* symbols, EvalContext ctx);
// EvaluatorResult evaluate(ASTNode* node);// old function (for compatibility)
// void evaluator_color_reset(ExecutionContext* ctx);
// void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color);
// void evaluator_color_apply_current(ExecutionContext* ctx);
//===================================================================


// ============================================
// HELPER FUNCTIONS (static)
// ============================================

static EvaluatorResult create_success_result_bool(int value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_BOOL;
    result.value.boolean = value;
    result.line = line;      
    result.column = column;  
    return result;    
}

static EvaluatorResult create_success_result_number(double value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_NUMBER;
    result.value.number = value;
    result.line = line;      
    result.column = column;  
    return result;
}

static EvaluatorResult create_success_result_string(const char* value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_STRING;
    strncpy(result.value.string, value, sizeof(result.value.string) - 1);
    result.value.string[sizeof(result.value.string) - 1] = '\0';
    result.line = line;
    result.column = column;
    return result;
}

static EvaluatorResult create_error_result(const char* message, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_ERROR;
    snprintf(result.error_message, sizeof(result.error_message), 
             "%s[%d:%d] %s%s", COLOR_ERROR, line, column, message, COLOR_RESET);
    result.line = line;
    result.column = column;
    return result;
}

// printf-style version for formatted messages
static EvaluatorResult create_error_result_fmt(int line, int column, 
                                              const char* format, ...)
{
    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    return create_error_result(message, line, column);
}


// =================================================
// CORES
// =================================================

// Reseta a cor atual
static void reset_current_color(void)
{
    current_color_global = "";
    if (colors_enabled_global) {
        printf("%s", COLOR_RESET);
    }
}

// Aplica uma cor (se diferente da atual)
static void apply_color(const char* ansi_color)
{
    if (!colors_enabled_global) return;
    
    // Se é nocolor (reset)
    if (ansi_color[0] == '\0' || strcmp(ansi_color, "\033[0m") == 0)
    {
        reset_current_color();
        return;
    }
    
    // Se é a mesma cor já aplicada, não faz nada
    if (strcmp(current_color_global, ansi_color) == 0)
    {
        return;
    }
    
    // Aplica a nova cor
    printf("%s", ansi_color);
    current_color_global = ansi_color;
}

// Verifica se o terminal suporta cores (detecção simples)
static void detect_color_support(void)
{
    // Por enquanto assume que sim
    // Futuro: verificar variáveis de ambiente TERM, NO_COLOR, etc.
    colors_enabled_global = 1;
}


// =================================================
// WIDTH E ALIGNMENT
// =================================================

// Função auxiliar para contar caracteres UTF-8
static int count_utf8_chars(const char* str)
{
    int count = 0;
    unsigned char *p = (unsigned char *)str;
    
    while (*p)
    {
        if ((*p & 0xC0) != 0x80) // (*p & 0b11000000) != 0b10000000; Não é um byte de continuação
        { 
            count++;
        }
        p++;
    }
    return count;
}

// Converte TokenType para AlignmentType
static AlignmentType token_to_alignment(TokenType token_type)
{
    switch (token_type)
    {
        case TOKEN_LEFT:   return ALIGN_LEFT;
        case TOKEN_RIGHT:  return ALIGN_RIGHT;
        case TOKEN_CENTER: return ALIGN_CENTER;
        default:           return ALIGN_LEFT; // Padrão
    }
}

// Reseta o formato atual
static void reset_format(ExecutionContext* ctx)
{
    if (ctx)
    {
        ctx->format.width = 0;
        ctx->format.align = ALIGN_LEFT;
        ctx->format.has_format = 0; 
    }
}

// Atualiza o formato baseado em um nó AST
static void update_format_from_node(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return;
    
    switch (node->type)
    {
        case NODE_WIDTH:
            ctx->format.width = node->data.width.value;
            ctx->format.has_format = 1;
            break;
            
        case NODE_ALIGNMENT:
            ctx->format.align = token_to_alignment(node->data.alignment.alignment_type);
            ctx->format.has_format = 1;
            break;
            
        default:
            // Não é um nó de formatação
            break;
    }
}

// Aplica formatação (width e alinhamento) a uma string
static void apply_format(const char* str, OutputFormat* format)
{
    if (!format || !format->has_format || format->width <= 0)
    {
        printf("%s", str);
        return;
    }
    
    int str_len = count_utf8_chars(str);
    int width = format->width;
    
    if (str_len >= width)
    {
        // Se a string for maior ou igual à largura, imprime sem formatação
        printf("%s", str);
        return;
    }
    
    int padding = width - str_len;
    
    switch (format->align)
    {
        case ALIGN_LEFT:
            printf("%s", str);
            for (int i = 0; i < padding; i++) putchar(' ');
            break;
            
        case ALIGN_RIGHT:
            for (int i = 0; i < padding; i++) putchar(' ');
            printf("%s", str);
            break;
            
        case ALIGN_CENTER:
        {
            int left_pad = padding / 2;
            int right_pad = padding - left_pad;
            for (int i = 0; i < left_pad; i++) putchar(' ');
            printf("%s", str);
            for (int i = 0; i < right_pad; i++) putchar(' ');
            break;
        }
    }
}

// Reseta formatação (pública)
void evaluator_reset_format(ExecutionContext* ctx)
{
    reset_format(ctx);
}

// Cria contexto de execução
ExecutionContext* execution_ctx_create(SymbolTable* symbols)
{
    ExecutionContext* ctx = A89ALLOC(sizeof(ExecutionContext));
    ctx->symbols = symbols;
    ctx->current_color = "";
    ctx->color_enabled = 1;
    ctx->should_break = 0;      
    ctx->should_continue = 0;  
    reset_format(ctx);    
    return ctx;
}

// Destrói contexto de execução
void execution_ctx_destroy(ExecutionContext* ctx)
{
    if (ctx)
    {
        a89free(ctx);
    }
}


// =================================================
// FUNÇÕES PARA INPUT
// =================================================

// Detecta se uma string pode ser convertida para número
static int is_numeric_string(const char* str)
{
    if (!str || *str == '\0') return 0;
    
    char* endptr;
    strtod(str, &endptr);
    
    // Se conseguiu converter tudo e não sobrou nada
    return (*endptr == '\0');
}

// Lê entrada do usuário com prompt
static char* read_user_input(const char* prompt)
{
    if (prompt && prompt[0] != '\0') {
        printf("%s", prompt);
        fflush(stdout);  // Garante que o prompt seja exibido antes de ler
    }
    
    static char buffer[BUFFER_SIZE];
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return NULL;  // Erro ou EOF
    }
    
    // Remove newline no final
    buffer[strcspn(buffer, "\n")] = '\0';
    
    return buffer;
}

// Avalia statement input
static int evaluate_input_stmt(ASTNode* node, SymbolTable* symbols)
{
    if (!node || node->type != NODE_INPUT || !symbols)
    {
        printf("Evaluator error: expected input statement node\n");
        return 0;
    }
    
    const char* prompt = node->data.input_stmt.prompt;
    const char* var_name = node->data.input_stmt.var_name;
    
    // Lê entrada do usuário
    char* input = read_user_input(prompt);
    if (!input)
    {
        printf("Evaluator error: reading input\n");
        return 0;
    }

    if(!strcmp(input, "true") || !strcmp(input, "false") )
    {
        if(strcmp(input, "true") == 0) {
            if (!symbol_table_set_bool(symbols, var_name, 1)) {
                printf("Evaluator error: assigning boolean to '%s'\n", var_name);
                return 0;
            }
        }
        else if(strcmp(input, "false") == 0) {
            if (!symbol_table_set_bool(symbols, var_name, 0)) {
                printf("Evaluator error: assigning boolean to '%s'\n", var_name);
                return 0;
            }
        }
    }    
    else if (is_numeric_string(input))
    {
        double value = atof(input);
        if (!symbol_table_set_number(symbols, var_name, value))
        {
            printf("Evaluator error: assigning number to '%s'\n", var_name);
            return 0;
        }
    }
    else
    {
        if (!symbol_table_set_string(symbols, var_name, input))
        {
            printf("Evaluator error: assigning string to '%s'\n", var_name);
            return 0;
        }
        //printf("OK: %s = \"%s\" (from input)\n", var_name, input);
    }
    
    return 1;
}

// ==================================================================
// Executa uma lista de statements
// ==================================================================
static int execute_stmt_list(ASTNode* node, SymbolTable* symbols)
{
    if (!node || node->type != NODE_STATEMENT_LIST)
    {
        printf("Evaluator error: expected statement list node\n");
        return 0;
    }
    
    StatementListData* list = &node->data.stmt_list;
    int all_success = 1;
    
    // Executa cada statement em sequência
    for (int i = 0; i < list->count; i++)
    {
        ASTNode* stmt = list->statements[i];
        int success = execute_stmt(stmt, symbols);
        
        if (!success)
        {
            all_success = 0;
            // Não para no primeiro erro? Decisão de design.
            // Por enquanto, continua executando os outros.
        }
    }
    
    return all_success;
}

// ============================================
// EXECUTE STATEMENT (uses CTX_ANY by default)
// ============================================
static int execute_stmt(ASTNode* node, SymbolTable* symbols)
{
    if (!node) return 0;
    
    switch (node->type)
    {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->data.assignment.var_name;
            ASTNode* value_node = node->data.assignment.value;
            
            // Evaluate value (any type)
            EvaluatorResult value_result = evaluate_expr(
                value_node, symbols, CTX_ANY);
            
            if (value_result.type == RESULT_ERROR)
            {
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            // Store based on type
            if (value_result.type == RESULT_STRING)
            {
                if (!symbol_table_set_string(symbols, var_name, value_result.value.string))
                {
                    printf("Evaluator error: assigning string to '%s'\n", var_name);
                    return 0;
                }
            }
            else if (value_result.type == RESULT_NUMBER)
            {
                if (!symbol_table_set_number(symbols, var_name, value_result.value.number))
                {
                    printf("Evaluator error: assigning number to '%s'\n", var_name);
                    return 0;
                }
            }
            else if (value_result.type == RESULT_BOOL)
             {  
                if (!symbol_table_set_bool(symbols, var_name, value_result.value.boolean))
                {
                    printf("Evaluator error: assigning boolean to '%s'\n", var_name);
                    return 0;
                }
            }
            return 1;
        }
            
        case NODE_BOOL:
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            // Evaluate for display (any type)
            EvaluatorResult result = evaluate_expr(node, symbols, CTX_ANY);
            if(result.type == RESULT_ERROR)
            {
                printf("%s\n", result.error_message);
                return 0;
            }
            else
            {
                switch(result.type)
                {
                    case RESULT_STRING:
                        printf("\"%s\"\n", result.value.string);
                        break;
                    case RESULT_NUMBER:
                        printf("%g\n", result.value.number);
                        break;
                    case RESULT_BOOL:
                    {
                        if(result.value.boolean == 1){
                            printf("true\n");
                        }
                        else{
                            printf("false\n");
                        }
                        break;
                    }
                }
                return 1;
            }
        }
            
        case NODE_STRING:
        {
            // Standalone string
            printf("= \"%s\"\n", node->data.string.value);
            return 1;
        }

        case NODE_PRINT:
            return evaluate_print_stmt(node, symbols);

        case NODE_COLOR:
            // Comando nocolor sozinho (ex: "nocolor" como statement)
            // Apenas reseta a cor
            if (node->data.color.color_token_id == 0) {  // CLR_NOCOLOR
                reset_current_color();
                return 1;
            }
            // Outras cores sozinhas não fazem sentido como statements
            printf("%s[%d:%d] Evaluator warning: color command without print has no effect%s\n",
                   COLOR_WARNING, node->line, node->column, COLOR_RESET);
            return 1;


        case NODE_STATEMENT_LIST:  
            return execute_stmt_list(node, symbols);

        case NODE_INPUT:
            return evaluate_input_stmt(node, symbols);

        case NODE_COMPARISON_OP:
        case NODE_LOGICAL_OP:
        case NODE_NOT_LOGICAL_OP:
        {
            // Evaluate logical/comparison expression
            EvaluatorResult result = evaluate_expr(node, symbols, CTX_ANY);
            
            if (result.type == RESULT_ERROR)
            {
                printf("%s\n", result.error_message);
                return 0;
            }
            else
            {
                // Print the result
                switch(result.type)
                {
                    case RESULT_BOOL:
                    {
                        if(result.value.boolean == 1){
                            printf("true\n");
                        }
                        else{
                            printf("false\n");
                        }
                        break;
                    }
                    case RESULT_NUMBER:
                        printf("%g\n", result.value.number);
                        break;
                    case RESULT_STRING:
                        printf("\"%s\"\n", result.value.string);
                        break;
                    default:
                        break;
                }
                return 1;
            }
        }

        case NODE_IF:
            return execute_if_stmt(node, symbols);

        case NODE_WHILE:
        {
            ExecutionContext* ctx = execution_ctx_create(symbols);
            if (!ctx) return 0;
            
            int result = execute_while_stmt_with_ctx(node, ctx);
            
            execution_ctx_destroy(ctx);

            return result;
        }

        case NODE_BREAK:
            printf("Evaluator error: 'break' outside of loop\n");
            return 0;

        case NODE_CONTINUE:
            printf("Evaluator error: 'continue' outside of loop\n");
            return 0;
            
        default:
            printf("Evaluator error: unsupported statement type: %d\n", node->type);
            return 0;
    }
}

// Função de execução com contexto 
static int execute_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return 0;
    
    switch (node->type) {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->data.assignment.var_name;
            ASTNode* value_node = node->data.assignment.value;
            
            EvaluatorResult value_result = evaluate_expr(
                value_node, ctx->symbols, CTX_ANY);
            
            if (value_result.type == RESULT_ERROR) {
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            if (value_result.type == RESULT_STRING) {
                if (!symbol_table_set_string(ctx->symbols, var_name, value_result.value.string)) {
                    printf("Evaluator error: assigning string to '%s'\n", var_name);
                    return 0;
                }
            } else {
                if (!symbol_table_set_number(ctx->symbols, var_name, value_result.value.number)) {
                    printf("Evaluator error: assigning number to '%s'\n", var_name);
                    return 0;
                }
            }
            return 1;
        }
            
        case NODE_PRINT:
            return evaluate_print_with_ctx(node, ctx);
            
        case NODE_COLOR:
            // Comando nocolor sozinho
            if (node->data.color.color_token_id == 0) {
                evaluator_color_reset(ctx);
                return 1;
            }
            printf("%s[%d:%d] Evaluator warning: color command without print has no effect%s\n",
                   COLOR_WARNING, node->line, node->column, COLOR_RESET);
            return 1;
            

        case NODE_BOOL:
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            EvaluatorResult result = evaluate_expr(node, ctx->symbols, CTX_ANY);
            if (result.type == RESULT_ERROR)
            {
                printf("%s\n", result.error_message);
                return 0;
            }
            else
            {
                switch(result.type)
                {
                    case RESULT_STRING:
                        printf("\"%s\"\n", result.value.string);
                        break;
                    case RESULT_NUMBER:
                        printf("%g\n", result.value.number);
                        break;
                    case RESULT_BOOL:
                    {
                        if(result.value.boolean == 1){
                            printf("true\n");
                        }
                        else{
                            printf("false\n");
                        }
                        break;
                    }
                }
                return 1;
            }
        }
            
        case NODE_STRING:
            printf("\"%s\"\n", node->data.string.value);
            return 1;
            
        case NODE_STATEMENT_LIST:
            return execute_stmt_list(node, ctx->symbols);

        case NODE_IF:
            return execute_if_stmt_with_ctx(node, ctx);

        case NODE_WHILE:
            return execute_while_stmt_with_ctx(node, ctx);

        case NODE_BREAK:
            ctx->should_break = 1;
            return 1;

        case NODE_CONTINUE:
            ctx->should_continue = 1;
            return 1;
            
        default:
            printf("Evaluator error: unsupported statement type: %d\n", node->type);
            return 0;
    }
}


// ============================================
// EXECUTE PRINT STATEMENT
// ============================================

// Função pública para avaliar print com contexto
static int evaluate_print_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    return evaluate_print_stmt_with_format(node, ctx);
}

static int evaluate_print_stmt(ASTNode* node, SymbolTable* symbols)
{
    ExecutionContext* ctx = execution_ctx_create(symbols);
    if (!ctx) return 0;
    
    int result = evaluate_print_stmt_with_format(node, ctx);
    
    execution_ctx_destroy(ctx);
    return result;
}

static int evaluate_print_stmt_with_format(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || node->type != NODE_PRINT || !ctx)
    {
        printf("Evaluator error: expected print statement node\n");
        return 0;
    }
    
    PrintStatementData* print_data = &node->data.print_stmt;
    int printed_something = 0;

    // Se não tem itens (print vazio) → linha em branco
    if (print_data->count == 0)
    {
        printf("\n");
        return 1;
    }
    
    // Salva o formato original
    OutputFormat original_format = ctx->format;
    
    // Avalia e imprime cada item
    for (int i = 0; i < print_data->count; i++)
    {
        ASTNode* item_node = print_data->items[i];

        // ============================================
        // CASOS ESPECIAIS DE FORMATAÇÃO
        // ============================================
        
        // 1. NODE_COLOR (aplica cor)
        if (item_node->type == NODE_COLOR)
        {
            evaluator_color_set(ctx, item_node->data.color.ansi_color);
            continue;
        }
        
        // 2. NODE_WIDTH e NODE_ALIGNMENT (atualiza formato)
        if (item_node->type == NODE_WIDTH || item_node->type == NODE_ALIGNMENT)
        {
            update_format_from_node(item_node, ctx);
            continue;
        }
        
        // ======================================================
        // CASO NORMAL: Expressão (número, string, variável, etc)
        // ======================================================
        EvaluatorResult result = evaluate_expr(item_node, ctx->symbols, CTX_ANY);
        if (result.type == RESULT_ERROR) {
            printf("%s\n", result.error_message);
            return 0;
        }
        
        // Converte para string
        char buffer[BUFFER_SIZE];
        if (result.type == RESULT_BOOL)
        {
            // Trata booleano
            snprintf(buffer, sizeof(buffer), "%s", 
                     result.value.boolean ? "true" : "false");
        }
        else if (result.type == RESULT_STRING)
        {
            snprintf(buffer, sizeof(buffer), "%s", result.value.string);
        }
        else
        {
            // Formata número sem zeros desnecessários
            double num = result.value.number;
            
            if (fabs(num - (int)num) < EPSILON) {
                snprintf(buffer, sizeof(buffer), "%d", (int)num);
            } else {
                // Remove zeros à direita
                char temp[NUMBER_SIZE];
                snprintf(temp, sizeof(temp), "%.10g", num);
                
                // Remove .00000 no final
                char* dot = strchr(temp, '.');
                if (dot) {
                    char* end = temp + strlen(temp) - 1;
                    while (end > dot && *end == '0') {
                        *end = '\0';
                        end--;
                    }
                    if (*(end) == '.') {
                        *end = '\0';
                    }
                }
                snprintf(buffer, sizeof(buffer), "%s", temp);
            }
        }
        
        // Aplica formatação se estiver ativa
        if (ctx->format.has_format && ctx->format.width > 0) {
            apply_format(buffer, &ctx->format);
            
            // Reseta formato após aplicar (formato é "consumível")
            reset_format(ctx);
        } else {
            printf("%s", buffer);
        }
        
        // Adiciona espaço entre itens (exceto após o último)
        if (i < print_data->count - 1) {
            printf(" ");
        }
        
        printed_something = 1;
    }
    
    // Restaura formato original (se necessário)
    // ctx->format = original_format;
    
    // Quebra linha se tem newline=1
    if (print_data->newline) {
        printf("\n");
    }
    
    return printed_something ? 1 : 0;
}

static int execute_if_stmt(ASTNode* node, SymbolTable* symbols)
{
    if (!node || !symbols) return 0;
    
    ExecutionContext* ctx = execution_ctx_create(symbols);
    if (!ctx) return 0;
    
    int result = execute_if_stmt_with_ctx(node, ctx);
    
    execution_ctx_destroy(ctx);
    return result;
}

static int execute_if_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return 0;
    
    // Evaluate condition
    EvaluatorResult cond_result = evaluate_expr(
                                  node->data.if_stmt.condition,
                                  ctx->symbols, CTX_BOOL);
    
    if (cond_result.type == RESULT_ERROR)
    {
        printf("%s\n", cond_result.error_message);
        return 0;
    }
    
    // Execute appropriate branch
    if (cond_result.value.boolean)
    {
        // Execute THEN body
        return execute_stmt_with_ctx(node->data.if_stmt.then_body, ctx);
    }
    else if (node->data.if_stmt.else_body)
    {
        // Execute ELSE body
        return execute_stmt_with_ctx(node->data.if_stmt.else_body, ctx);
    }
    
    // No else body, just return success
    return 1;
}

static int execute_while_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx)
    {
        return 0;
    }
    
    ASTNode* condition = node->data.while_stmt.condition;
    ASTNode* body = node->data.while_stmt.body;
    
    // Loop enquanto condição for verdadeira
    while (1)
    {
        // Avalia condição
        EvaluatorResult cond_result = evaluate_expr(
            condition, ctx->symbols, CTX_BOOL);
        
        if (cond_result.type == RESULT_ERROR)
        {
            printf("%s\n", cond_result.error_message);
            return 0;
        }
        
        // Se condição for falsa, sai do loop
        if (!cond_result.value.boolean)
        {
            break;
        }
        
        // Executa body
        int success = execute_stmt_with_ctx(body, ctx);
        if (!success)
        {
            return 0;
        }
        
        // Verifica se deve sair do loop (break)
        if (ctx->should_break)
        {
            ctx->should_break = 0;  // Reset
            break;
        }
        
        // Verifica se deve pular para próxima iteração (continue)
        if (ctx->should_continue)
        {
            ctx->should_continue = 0;  // Reset
            continue;
        }
    }
    
    return 1;
}



// ===================================================
// EVALUATE PROGRAM 
// Função principal para avaliar um programa completo
// ===================================================
int evaluate_program(ASTNode* node, SymbolTable* symbols) {
    if (!node) return 0;
    
    if (node->type == NODE_STATEMENT_LIST) {
        return execute_stmt_list(node, symbols);
    }
    else {
        // Programa com apenas um statement (backward compatibility)
        return execute_stmt(node, symbols);
    }
}


// ============================================
// EVALUATE EXPRESSIONS (with context)
// ============================================
EvaluatorResult evaluate_expr(ASTNode* node, SymbolTable* symbols, EvalContext ctx)
{
    if (node == NULL)
    {
        return create_error_result("Evaluator error: AST node is null", 0, 0);
    }
    
    switch (node->type)
    {

        case NODE_BOOL:
        {
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: boolean cannot be used as string");
            }
            return create_success_result_bool(node->data.boolean.value, 
                                               node->line, node->column);
        }

        case NODE_NUMBER:
        {
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: number cannot be used as string");
            }
            return create_success_result_number(node->data.number.value, 
                                               node->line, node->column);
        }
            
        case NODE_VARIABLE:
        {
            const char* var_name = node->data.variable.var_name;
            
            // Check if exists
            if (!symbol_table_exists(symbols, var_name))
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: variable '%s' not declared. Use 'let %s = value'", 
                     var_name, var_name);
            }
            
            // Try as number
            double num_value;
            if (symbol_table_get_number(symbols, var_name, &num_value))
            {
                if (ctx == CTX_STRING)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: variable '%s' is a number, cannot be used as string", 
                         var_name);
                }
                return create_success_result_number(num_value, node->line, node->column);
            }
            
            // Try as string
            char str_value[STRING_SIZE];
            if (symbol_table_get_string(symbols, var_name, str_value, sizeof(str_value)))
            {
                if (ctx == CTX_NUMBER)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: variable '%s' is a string, cannot be used in mathematical operation", 
                         var_name);
                }
                return create_success_result_string(str_value, node->line, node->column);
            }

            // Try as boolean
            int bool_value;
            if (symbol_table_get_bool(symbols, var_name, &bool_value))
            {
                if (ctx == CTX_NUMBER)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: variable '%s' is a boolean, cannot be used in mathematical operation", 
                         var_name);
                }
                if (ctx == CTX_STRING)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: variable '%s' is a boolean, cannot be used as string", 
                         var_name);
                }
                return create_success_result_bool(bool_value, node->line, node->column);
            }      
            
            // Should not reach here
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: internal error: unknown variable type '%s'", var_name);
        }
            
        case NODE_BINARY_OP:
        {
            // Binary operations always expect numbers (for now)
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: mathematical operation cannot be used as string");
            }
            
            // Evaluate operands in number context
            EvaluatorResult left_result = evaluate_expr(
                node->data.binaryop.left, symbols, CTX_NUMBER);
            if (left_result.type == RESULT_ERROR) return left_result;
            
            EvaluatorResult right_result = evaluate_expr(
                node->data.binaryop.right, symbols, CTX_NUMBER);
            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Both must be numbers
            if (left_result.type == RESULT_STRING || right_result.type == RESULT_STRING )
            {
                return create_error_result("Evaluator error: mathematical operation with string", 
                                         node->line, node->column);
            }

            // Both must be numbers
            if (left_result.type == RESULT_BOOL || right_result.type == RESULT_BOOL )
            {
                return create_error_result("Evaluator error: mathematical operation with boolean", 
                                         node->line, node->column);
            }
            
            double result;
            switch (node->data.binaryop.operator)
            {
                case '+': 
                    result = left_result.value.number + right_result.value.number; 
                    break;
                case '-': 
                    result = left_result.value.number - right_result.value.number; 
                    break;
                case '*': 
                    result = left_result.value.number * right_result.value.number; 
                    break;
                case '/': 
                    if (fabs(right_result.value.number) < EPSILON)
                    {
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: division by zero");
                    }
                    result = left_result.value.number / right_result.value.number; 
                    break;
                default: 
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: invalid operator '%c'", node->data.binaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_UNARY_OP:
        {
            // Unary operations always expect numbers
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: unary operator cannot be applied to string");
            }
            
            EvaluatorResult operand_result = evaluate_expr(
                node->data.unaryop.operand, symbols, CTX_NUMBER);
            if (operand_result.type == RESULT_ERROR)  return operand_result;
            
            if (operand_result.type == RESULT_STRING )
            {
                return create_error_result("Evaluator error: unary operator '-' applied to string", 
                                         node->line, node->column);
            }
            
            double result;
            switch (node->data.unaryop.operator)
            {
                case '+':
                    result = operand_result.value.number;  // +x = x
                    break;
                case '-':
                    result = -operand_result.value.number; // -x
                    break;
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: invalid unary operator '%c'", node->data.unaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_STRING:
        {
            if (ctx == CTX_NUMBER)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: string cannot be used as number");
            }
            return create_success_result_string(node->data.string.value, 
                                               node->line, node->column);
        }

        case NODE_STATEMENT_LIST:
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: statement list cannot be used as expression");

        case NODE_COMPARISON_OP:
        {
            // Operações de comparação: ==, !=, <, >, <=, >=
            // Resultado é sempre booleano
            
            // Avalia lado esquerdo (pode ser número ou booleano)
            EvaluatorResult left_result = evaluate_expr(
                node->data.logicalop.left, symbols, CTX_ANY);
            if (left_result.type == RESULT_ERROR) return left_result;
            
            // Avalia lado direito (pode ser número ou booleano)
            EvaluatorResult right_result = evaluate_expr(
                node->data.logicalop.right, symbols, CTX_ANY);
            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Ambos devem ser do mesmo tipo (número ou booleano)
            if (left_result.type != right_result.type)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: type mismatch in comparison: cannot compare %s with %s",
                     (left_result.type == RESULT_NUMBER ? "number" : "boolean"),
                     (right_result.type == RESULT_NUMBER ? "number" : "boolean"));
            }
            
            // Strings não podem ser comparadas (por enquanto)
            if (left_result.type == RESULT_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: string comparison not supported");
            }
            
            int comparison_result = 0;
            
            // Comparação de números
            if (left_result.type == RESULT_NUMBER)
            {
                double left = left_result.value.number;
                double right = right_result.value.number;
                
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result = (fabs(left - right) < EPSILON) ? 1 : 0;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = (fabs(left - right) >= EPSILON) ? 1 : 0;
                        break;
                    case OP_LESS:
                        comparison_result = (left < right) ? 1 : 0;
                        break;
                    case OP_GREATER:
                        comparison_result = (left > right) ? 1 : 0;
                        break;
                    case OP_LESS_EQUAL:
                        comparison_result = (left <= right) ? 1 : 0;
                        break;
                    case OP_GREATER_EQUAL:
                        comparison_result = (left >= right) ? 1 : 0;
                        break;
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: invalid comparison operator");
                }
            }
            // Comparação de booleanos
            else if (left_result.type == RESULT_BOOL)
            {
                int left = left_result.value.boolean;
                int right = right_result.value.boolean;
                
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result = (left == right) ? 1 : 0;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = (left != right) ? 1 : 0;
                        break;
                    // Outros operadores não fazem sentido para booleanos
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: operator not supported for boolean values");
                }
            }
            
            return create_success_result_bool(comparison_result, 
                                             node->line, node->column);
        }

        case NODE_LOGICAL_OP:
        {
            // Operações lógicas: AND, OR
            // Resultado é sempre booleano
            
            LogicalOperator op = node->data.logicalop.operator;
            
            // Avalia lado esquerdo (deve ser booleano)
            EvaluatorResult left_result = evaluate_expr(
                node->data.logicalop.left, symbols, CTX_BOOL);
            if (left_result.type == RESULT_ERROR) return left_result;
            
            // Verifica se é booleano
            if (left_result.type != RESULT_BOOL)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: logical operator expects boolean, got %s",
                     (left_result.type == RESULT_NUMBER ? "number" : "string"));
            }
            
            int left = left_result.value.boolean;
            
            // OTIMIZAÇÃO: Short-circuit evaluation
            // Se é AND e left é false, não precisa avaliar right
            if (op == OP_AND && !left)
            {
                return create_success_result_bool(0, node->line, node->column);
            }
            
            // Se é OR e left é true, não precisa avaliar right
            if (op == OP_OR && left)
            {
                return create_success_result_bool(1, node->line, node->column);
            }
            
            // Avalia lado direito (deve ser booleano)
            EvaluatorResult right_result = evaluate_expr(
                node->data.logicalop.right, symbols, CTX_BOOL);
            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Verifica se é booleano
            if (right_result.type != RESULT_BOOL)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: logical operator expects boolean, got %s",
                     (right_result.type == RESULT_NUMBER ? "number" : "string"));
            }
            
            int right = right_result.value.boolean;
            int logical_result = 0;
            
            switch (op)
            {
                case OP_AND:
                    logical_result = left && right;
                    break;
                case OP_OR:
                    logical_result = left || right;
                    break;
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: invalid logical operator");
            }
            
            return create_success_result_bool(logical_result, 
                                             node->line, node->column);
        }

        case NODE_NOT_LOGICAL_OP:
        {
            // Operação lógica unária: NOT (!)
            // Resultado é sempre booleano
            
            // Avalia operando (deve ser booleano)
            EvaluatorResult operand_result = evaluate_expr(
                node->data.notop.operand, symbols, CTX_BOOL);
            if (operand_result.type == RESULT_ERROR) return operand_result;
            
            // Verifica se é booleano
            if (operand_result.type != RESULT_BOOL)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: NOT operator expects boolean, got %s",
                     (operand_result.type == RESULT_NUMBER ? "number" : "string"));
            }
            
            int operand = operand_result.value.boolean;
            int not_result = !operand;  // Inverte o valor
            
            return create_success_result_bool(not_result, 
                                             node->line, node->column);
        }
            
        default:
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: unsupported node type: %d", node->type);
    }
}


// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node)
{
    SymbolTable* symbols = symbol_table_create();
    if (!symbols)
    {
        return create_error_result("Evaluator error: could not create symbol table", 0, 0);
    }
    
    EvaluatorResult result = evaluate_expr(node, symbols, CTX_ANY);
    symbol_table_destroy(symbols);
    return result;
}


// evaluator.c - ADICIONAR estas implementações

// Funções de gerenciamento de cores com contexto
void evaluator_color_reset(ExecutionContext* ctx)
{
    if (ctx) {
        ctx->current_color = "";
        if (ctx->color_enabled) {
            printf("%s", COLOR_RESET);
        }
    } else {
        reset_current_color();
    }
}

void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color)
{
    if (!ctx) {
        apply_color(ansi_color);
        return;
    }
    
    if (!ctx->color_enabled) return;
    
    // Se é nocolor (reset)
    if (ansi_color[0] == '\0' || strcmp(ansi_color, "\033[0m") == 0) {
        evaluator_color_reset(ctx);
        return;
    }
    
    // Se é a mesma cor já aplicada, não faz nada
    if (strcmp(ctx->current_color, ansi_color) == 0) {
        return;
    }
    
    // Aplica a nova cor
    printf("%s", ansi_color);
    ctx->current_color = ansi_color;
}

void evaluator_color_apply_current(ExecutionContext* ctx)
{
    if (!ctx) return;
    
    if (ctx->color_enabled && ctx->current_color && ctx->current_color[0] != '\0') {
        printf("%s", ctx->current_color);
    }
}

#ifdef TEST
#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "symbol_table.h"

int main()
{
    setup_utf8();
    
    printf("%s=== TESTE EVALUATOR v0.5.3 - loop while ===%s\n\n", 
           COLOR_HEADER, COLOR_RESET);
    
    char* testes[] =
    {

        "let x = 0\n"
        "while (x < 5) do\n"
        "    print x nl\n"
        "    let x = x + 1\n"
        "end while",

        "let x = 0\n"
        "while (x < 10) do\n"
        "    if (x == 5) then\n"
        "        break\n"
        "    end if\n"
        "    print x nl\n"
        "    let x = x + 1\n"
        "end while\n"

        "print \"Saiu do loop\" nl",

        "let x = 0\n"
        "while (x < 5) do\n"
        "    let x = x + 1\n"
        "    if (x == 3) then\n"
        "        continue\n"
        "    end if\n"
        "    print x nl\n"
        "end while",

        "let i = 0\n"
        "while (i < 3) do\n"
        "    let j = 0\n"
        "    while (j < 3) do\n"
        "        print i " " j nl\n"
        "        let j = j + 1\n"
        "    end while\n"
        "    let i = i + 1\n"
        "end while",

        "let x = 0\n"
        "while (x < 10) do\n"
        "    let x = x + 1\n"
        "    if (x == 3) then\n"
        "        continue\n"
        "    end if\n"
        "    if (x == 7) then\n"
        "        break\n"
        "    end if\n"
        "    print x nl\n"
        "end while"

    };
    
    int num_testes = sizeof(testes) / sizeof(testes[0]);
    SymbolTable* symbols = symbol_table_create();
    
    for (int i = 0; i < num_testes; i++)
    {
        printf("%s=== Teste %d: '%s' ===%s\n", 
               COLOR_HEADER, i+1, testes[i], COLOR_RESET);
        
        Lexer lexer;
        lexer_init(&lexer, testes[i]);
        
        ASTNode* ast = parse_single_stmt(&lexer);
        
        if (ast)
        {
            // BUG CORRIGIDO - TESTES FALHAVAM COM MENSAGEM:
            // Warning: remaining tokens not parsed
            // [1:9] Evaluator error: unsupported node type: 7
            // node type 7 é o NODE_WHILE
            // SOLUÇÃO:
            // Quando ast é um NODE_WHILE (ou qualquer statement),
            // evaluate_expr() não sabe como lidar.
            // evaluate_expr() não trata NODE_WHILE.
            // evaluate_expr() é para avaliar expressões (números, variáveis, operações matemáticas)
            // execute_stmt() é para executar statements (atribuições, loops, condicionais)
            // Um NODE_WHILE é um statement, não uma expressão.
            //
            // LINHA QUE PROVOCAVA O ERRO
            // EvaluatorResult result = evaluate_expr(ast, symbols, CTX_ANY);

            int result = evaluate_program(ast, symbols);
            
            if (!result)
            {
                printf("Error executing statement\n");
            }
        }
        else 
        {
            printf("Parse error\n");                
        }
            
        free_ast(ast);

        printf("\n");
        wait();
    }
    
    symbol_table_destroy(symbols);
    
    printf("\n%s=== TODOS OS TESTES COMPLETADOS ===%s\n", 
           COLOR_SUCCESS, COLOR_RESET);
    
    a89check_leaks();
    return 0;
}
#endif
// Fim de evaluator.c