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


//===================================================================
// FUNÇÕES DESTE ARQUIVO
//===================================================================
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
ExecutionContext* execution_context_create(SymbolTable* symbols);
void execution_context_destroy(ExecutionContext* ctx);
void evaluator_reset_format(ExecutionContext* ctx);
static int evaluate_print_statement_with_format(ASTNode* node, ExecutionContext* ctx);
int evaluate_print_with_context(ASTNode* node, ExecutionContext* ctx);


// FUNÇÕES PÚBLICAS

// int evaluate_program(ASTNode* node, SymbolTable* symbols);
// int execute_statement_list(ASTNode* node, SymbolTable* symbols);
// int execute_statement(ASTNode* node, SymbolTable* symbols);
// int evaluate_print_statement(ASTNode* node, SymbolTable* symbols);
// EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx);
// // Old function (for compatibility)
// EvaluatorResult evaluate(ASTNode* node);
//===================================================================


// ============================================
// HELPER FUNCTIONS (static)
// ============================================

static EvaluatorResult create_success_result_number(double value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.success = 1;
    result.is_string = 0;
    result.value.number = value;
    result.line = line;      
    result.column = column;  
    return result;
}

static EvaluatorResult create_success_result_string(const char* value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.success = 1;
    result.is_string = 1;
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

// Cria contexto de execução
ExecutionContext* execution_context_create(SymbolTable* symbols)
{
    ExecutionContext* ctx = A89ALLOC(sizeof(ExecutionContext));
    ctx->symbols = symbols;
    ctx->current_color = "";
    ctx->color_enabled = 1;
    reset_format(ctx);    
    return ctx;
}

// Destrói contexto de execução
void execution_context_destroy(ExecutionContext* ctx)
{
    if (ctx)
    {
        a89free(ctx);
    }
}

// Reseta formatação (pública)
void evaluator_reset_format(ExecutionContext* ctx)
{
    reset_format(ctx);
}

// ===================================================
// EVALUATE PROGRAM 
// Função principal para avaliar um programa completo
// ===================================================
int evaluate_program(ASTNode* node, SymbolTable* symbols) {
    if (!node) return 0;
    
    if (node->type == NODE_STATEMENT_LIST) {
        return execute_statement_list(node, symbols);
    }
    else {
        // Programa com apenas um statement (backward compatibility)
        return execute_statement(node, symbols);
    }
}


// ==================================================================
// Executa uma lista de statements
// ==================================================================
int execute_statement_list(ASTNode* node, SymbolTable* symbols)
{
    if (!node || node->type != NODE_STATEMENT_LIST)
    {
        printf("Error: expected statement list node\n");
        return 0;
    }
    
    StatementListData* list = &node->data.statementlist;
    int all_success = 1;
    
    // Executa cada statement em sequência
    for (int i = 0; i < list->count; i++)
    {
        ASTNode* stmt = list->statements[i];
        int success = execute_statement(stmt, symbols);
        
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
int execute_statement(ASTNode* node, SymbolTable* symbols)
{
    if (!node) return 0;
    
    switch (node->type)
    {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->data.assignment.var_name;
            ASTNode* value_node = node->data.assignment.value;
            
            // Evaluate value (any type)
            EvaluatorResult value_result = evaluate_expression(
                value_node, symbols, CTX_ANY);
            
            if (!value_result.success)
            {
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            // Store
            if (value_result.is_string)
            {
                if (!symbol_table_set_string(symbols, var_name, value_result.value.string))
                {
                    printf("Error assigning string to '%s'\n", var_name);
                    return 0;
                }
                //printf("OK: %s = \"%s\"\n", var_name, value_result.value.string);
            }
            else
            {
                if (!symbol_table_set_number(symbols, var_name, value_result.value.number))
                {
                    printf("Error assigning number to '%s'\n", var_name);
                    return 0;
                }
                //printf("OK: %s = %g\n", var_name, value_result.value.number);
            }
            return 1;
        }
            
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            // Evaluate for display (any type)
            EvaluatorResult result = evaluate_expression(node, symbols, CTX_ANY);
            if (result.success)
            {
                if (result.is_string)
                {
                    printf("= \"%s\"\n", result.value.string);
                }
                else
                {
                    printf("= %g\n", result.value.number);
                }
                return 1;
            }
            else
            {
                printf("%s\n", result.error_message);
                return 0;
            }
        }
            
        case NODE_STRING:
        {
            // Standalone string
            printf("= \"%s\"\n", node->data.string.value);
            return 1;
        }

        case NODE_PRINT:
            return evaluate_print_statement(node, symbols);

        case NODE_COLOR:
            // Comando nocolor sozinho (ex: "nocolor" como statement)
            // Apenas reseta a cor
            if (node->data.color.color_token_id == 0) {  // CLR_NOCOLOR
                reset_current_color();
                return 1;
            }
            // Outras cores sozinhas não fazem sentido como statements
            printf("%s[%d:%d] Warning: color command without print has no effect%s\n",
                   COLOR_WARNING, node->line, node->column, COLOR_RESET);
            return 1;


        case NODE_STATEMENT_LIST:  
            return execute_statement_list(node, symbols);
            
        default:
            printf("Unsupported statement type: %d\n", node->type);
            return 0;
    }
}

// ============================================
// EXECUTE PRINT STATEMENT
// ============================================

// Função pública para avaliar print com contexto
int evaluate_print_with_context(ASTNode* node, ExecutionContext* ctx)
{
    return evaluate_print_statement_with_format(node, ctx);
}

int evaluate_print_statement(ASTNode* node, SymbolTable* symbols)
{
    ExecutionContext* ctx = execution_context_create(symbols);
    if (!ctx) return 0;
    
    int result = evaluate_print_statement_with_format(node, ctx);
    
    execution_context_destroy(ctx);
    return result;
}

static int evaluate_print_statement_with_format(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || node->type != NODE_PRINT || !ctx)
    {
        printf("Error: expected print statement node\n");
        return 0;
    }
    
    PrintStatementData* print_data = &node->data.printstatement;
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
        EvaluatorResult result = evaluate_expression(item_node, ctx->symbols, CTX_ANY);
        if (!result.success) {
            printf("%s\n", result.error_message);
            return 0;
        }
        
        // Converte para string
        char buffer[BUFFER_SIZE];
        if (result.is_string) {
            snprintf(buffer, sizeof(buffer), "%s", result.value.string);
        } else {
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

// ============================================
// EVALUATE EXPRESSIONS (with context)
// ============================================
EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx)
{
    if (node == NULL)
    {
        return create_error_result("Error: AST node is null", 0, 0);
    }
    
    switch (node->type)
    {
        case NODE_NUMBER:
        {
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Number cannot be used as string");
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
                     "Variable '%s' not declared. Use 'let %s = value'", 
                     var_name, var_name);
            }
            
            // Try as number
            double num_value;
            if (symbol_table_get_number(symbols, var_name, &num_value))
            {
                if (ctx == CTX_STRING)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Variable '%s' is a number, cannot be used as string", 
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
                         "Variable '%s' is a string, cannot be used in mathematical operation", 
                         var_name);
                }
                return create_success_result_string(str_value, node->line, node->column);
            }
            
            // Should not reach here
            return create_error_result_fmt(node->line, node->column,
                 "Internal error: unknown variable type '%s'", var_name);
        }
            
        case NODE_BINARY_OP:
        {
            // Binary operations always expect numbers (for now)
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Mathematical operation cannot be used as string");
            }
            
            // Evaluate operands in number context
            EvaluatorResult left_result = evaluate_expression(
                node->data.binaryop.left, symbols, CTX_NUMBER);
            if (!left_result.success) return left_result;
            
            EvaluatorResult right_result = evaluate_expression(
                node->data.binaryop.right, symbols, CTX_NUMBER);
            if (!right_result.success) return right_result;
            
            // Both must be numbers
            if (left_result.is_string || right_result.is_string)
            {
                return create_error_result("Mathematical operation with string", 
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
                             "Division by zero");
                    }
                    result = left_result.value.number / right_result.value.number; 
                    break;
                default: 
                    return create_error_result_fmt(node->line, node->column,
                         "Invalid operator '%c'", node->data.binaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_UNARY_OP:
        {
            // Unary operations always expect numbers
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Unary operator cannot be applied to string");
            }
            
            EvaluatorResult operand_result = evaluate_expression(
                node->data.unaryop.operand, symbols, CTX_NUMBER);
            if (!operand_result.success) return operand_result;
            
            if (operand_result.is_string)
            {
                return create_error_result("Unary operator '-' applied to string", 
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
                         "Invalid unary operator '%c'", node->data.unaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_STRING:
        {
            if (ctx == CTX_NUMBER)
            {
                return create_error_result_fmt(node->line, node->column,
                     "String cannot be used as number");
            }
            return create_success_result_string(node->data.string.value, 
                                               node->line, node->column);
        }

        case NODE_STATEMENT_LIST:
            return create_error_result_fmt(node->line, node->column,
                 "Statement list cannot be used as expression");
            
        default:
            return create_error_result_fmt(node->line, node->column,
                 "Unsupported node type: %d", node->type);
    }
}


// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node)
{
    SymbolTable* symbols = symbol_table_create();
    if (!symbols)
    {
        return create_error_result("Error: could not create symbol table", 0, 0);
    }
    
    EvaluatorResult result = evaluate_expression(node, symbols, CTX_ANY);
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

// Função de execução com contexto (falta implementar)
int execute_statement_with_context(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return 0;
    
    switch (node->type) {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->data.assignment.var_name;
            ASTNode* value_node = node->data.assignment.value;
            
            EvaluatorResult value_result = evaluate_expression(
                value_node, ctx->symbols, CTX_ANY);
            
            if (!value_result.success) {
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            if (value_result.is_string) {
                if (!symbol_table_set_string(ctx->symbols, var_name, value_result.value.string)) {
                    printf("Error assigning string to '%s'\n", var_name);
                    return 0;
                }
            } else {
                if (!symbol_table_set_number(ctx->symbols, var_name, value_result.value.number)) {
                    printf("Error assigning number to '%s'\n", var_name);
                    return 0;
                }
            }
            return 1;
        }
            
        case NODE_PRINT:
            return evaluate_print_with_context(node, ctx);
            
        case NODE_COLOR:
            // Comando nocolor sozinho
            if (node->data.color.color_token_id == 0) {
                evaluator_color_reset(ctx);
                return 1;
            }
            printf("%s[%d:%d] Warning: color command without print has no effect%s\n",
                   COLOR_WARNING, node->line, node->column, COLOR_RESET);
            return 1;
            
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            EvaluatorResult result = evaluate_expression(node, ctx->symbols, CTX_ANY);
            if (result.success) {
                if (result.is_string) {
                    printf("= \"%s\"\n", result.value.string);
                } else {
                    printf("= %g\n", result.value.number);
                }
                return 1;
            } else {
                printf("%s\n", result.error_message);
                return 0;
            }
        }
            
        case NODE_STRING:
            printf("= \"%s\"\n", node->data.string.value);
            return 1;
            
        case NODE_STATEMENT_LIST:
            return execute_statement_list(node, ctx->symbols);
            
        default:
            printf("Unsupported statement type: %d\n", node->type);
            return 0;
    }
}

// Esta função também está declarada mas não implementada
int evaluate_print_statement_with_context(ASTNode* node, ExecutionContext* ctx)
{
    return evaluate_print_with_context(node, ctx);
}
// Fim de evaluator.c