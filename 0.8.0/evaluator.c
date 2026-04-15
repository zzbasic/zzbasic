// evaluator.c

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>

#include "zzdefs.h"
#include "utils.h"
#include "color.h"
#include "a89alloc.h"
#include "evaluator.h"
#include "zzarray.h"
#include "result.h"
#include "scope.h"
#include "zztext_wrapper.h"
#include "zzarray_wrapper.h"
#include "zzstring_wrapper.h"

//#include "debug.h"

// Hack temporário para acessar UserFunction (enquanto não criamos API completa)
// Isto é necessário porque a struct está definida em symbol_table.c
// TODO: Depois criar funções de acesso apropriadas
struct UserFunction
{
    char name[VARNAME_SIZE];
    char** parameters;
    int param_count;
    ASTNode* body;
    int scope_level;
};
typedef struct UserFunction UserFunction;

// Protótipo da função de execução de function def 
static int execute_function_def(ASTNode* node, ExecutionContext* ctx);

//===================================================================
// VARIAVEIS GLOBAIS
//===================================================================

// Cor atual sendo aplicada (estado global para sessão)
static const char* current_color_global = "";
static int colors_enabled_global = 1;


//===================================================================
// TABELA DE DISPATCH PARA FUNÇÕES BUILT-IN
//===================================================================
typedef struct
{
    const char* name;
    EvaluatorResult (*func)(EvaluatorResult* args, int arg_count, int line, int column,  ScopeStack* scope_stack);
} BuiltinFunction;

// Tabela com todas as funções built-in disponíveis
static BuiltinFunction builtins[] = {
    // Funções genericas
    {"len",      builtin_len},

    // Funções de Array
    {"push",     builtin_push},
    {"pop",      builtin_pop},
    {"is_empty", builtin_is_empty},
    {"get",      builtin_get},
    {"set",      builtin_set},
    {"insert",   builtin_insert},
    {"remove",   builtin_remove},
    {"swap",     builtin_swap},
    {"sort",     builtin_sort},
    {"rsort",    builtin_rsort},

    // Funções de Text
    {"text",   builtin_text},   
    {"load",   builtin_load},    
    {"save",   builtin_save},  

    // Funções de String
    {"substr",   builtin_substr},
    {"upper",    builtin_upper},
    {"lower",    builtin_lower},
    {"trim",     builtin_trim},
    {"split",    builtin_split},
    {"join",     builtin_join},  
    
    // Marcador de fim (OBRIGATÓRIO!)
    {NULL, NULL}
};


// CORES
static void reset_current_color(void);
static void apply_color(const char* ansi_color);
static void detect_color_support(void);

// WIDTH E ALIGNMENT
static int count_utf8_chars(const char* str);
static AlignmentType token_to_alignment(TokenType token_type);
static void reset_format(ExecutionContext* ctx);
static void update_format_from_node(ASTNode* node, ExecutionContext* ctx);
static void apply_format(const char* str, OutputFormat* format);

// FUNÇÕES PARA INPUT
static int is_numeric_string(const char* str);
static char* read_user_input(const char* prompt);

static int evaluate_input_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int evaluate_print_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int evaluate_print_stmt_with_format(ASTNode* node, ExecutionContext* ctx);

static int execute_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int execute_stmt_list_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int execute_if_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int execute_while_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int execute_do_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);
static int execute_for_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

static int execute_save_node(ASTNode* node, ExecutionContext* ctx);

static ModuleManager* module_manager_create(void);

static void module_manager_destroy(ModuleManager* manager);

static int module_manager_resize(ModuleManager* manager);

static int module_manager_load(ModuleManager* manager, const char* module_name);

static SymbolTable* module_manager_get_symbols(ModuleManager* manager, const char* module_name);

static int execute_function_def(ASTNode* node, ExecutionContext* ctx);


// FUNÇÕES PÚBLICAS

// int evaluate_program(ASTNode* node);

// EvaluatorResult evaluate_expr(ASTNode* node, ExecutionContext* ctx, EvalContext eval_ctx)

// EvaluatorResult evaluate(ASTNode* node);// old function (for compatibility)

// width e alignment
//ExecutionContext* execution_ctx_create(void);
//void execution_ctx_destroy(ExecutionContext* ctx);
//void evaluator_reset_format(ExecutionContext* ctx);

// Funções de gerenciamento de cores
// void evaluator_color_reset(ExecutionContext* ctx);
// void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color);
// void evaluator_color_apply_current(ExecutionContext* ctx);
//===================================================================


//===================================================================
// FUNCOES PARA GERENCIAMENTO DE MODULOS
//===================================================================
static ModuleManager* module_manager_create(void);
static void module_manager_destroy(ModuleManager* manager);
static int module_manager_resize(ModuleManager* manager);
static int module_manager_load(ModuleManager* manager, const char* module_name);
static SymbolTable* module_manager_get_symbols(ModuleManager* manager, 
                                        const char* module_name);


// =================================================
// FUNCOES PARA GERENCIMANTO DE CORES
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
// FUNCOES PARA GERENCIAMENTO DE WIDTH E ALIGNMENT
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

/********************************************************************
Lógica de Execução do INPUT

Fase 1: Aplicar Formatação (cor, width, alignment)
    1. Se color != NULL → aplicar cor
    2. Se width != NULL → armazenar width no contexto
    3. Se alignment != NULL → armazenar alignment no contexto
    4. Se set_nocolor == 1 → resetar cor

Fase 2: Imprimir Prompt com Formatação
    1. Se prompt não está vazio → imprimir com formatação
        - Usar apply_format() do PRINT
        - Respeitar width e alignment do contexto
    2. Sem quebra de linha (porque vai ler entrada)

Fase 3: Ler Entrada do Usuário
    1. fgets() para ler do stdin
    2. Remover newline

Fase 4: Armazenar na Variável
    1. Auto-detectar tipo:
        - "true"/"false" → boolean
        - Número → double
         - Resto → string
    2. Armazenar em symbol_table
********************************************************************/
static int evaluate_input_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || node->type != NODE_INPUT || !ctx)
    {
        printf("Evaluator error: expected input statement node\n");
        return 0;
    }
    
    InputStatementNode* input_stmt = &node->data.input_stmt;
    
    // ===================================================
    // FASE 1: Aplicar Formatação
    // ===================================================
    
    // 1.1 Aplicar cor (se existe)
    if (input_stmt->color != NULL)
    {
        evaluator_color_set(ctx, input_stmt->color->data.color.ansi_color);
    }
    
    // 1.2 Atualizar width (se existe)
    if (input_stmt->width != NULL)
    {
        update_format_from_node(input_stmt->width, ctx);
    }
    
    // 1.3 Atualizar alignment (se existe)
    if (input_stmt->alignment != NULL)
    {
        update_format_from_node(input_stmt->alignment, ctx);
    }
    
    // ===================================================
    // FASE 2: Imprimir Prompt com Formatação
    // ===================================================
    
    if (input_stmt->prompt[0] != '\0')
    {
        // Tem prompt → imprimir com formatação
        
        char buffer[BUFFER_SIZE];
        strncpy(buffer, input_stmt->prompt, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = '\0';
        
        // Aplica formatação se estiver ativa
        if (ctx->format.has_format && ctx->format.width > 0)
        {
            apply_format(buffer, &ctx->format);
            reset_format(ctx);
        }
        else
        {
            printf("%s", buffer);
        }
    }
    
    // Sem quebra de linha (vai ler entrada)
    fflush(stdout);
    
    // ===================================================
    // FASE 3: Ler Entrada do Usuário
    // ===================================================
    
    static char input_buffer[BUFFER_SIZE];
    
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
    {
        printf("Evaluator error: failed to read input\n");
        return 0;
    }
    
    // Remove newline
    input_buffer[strcspn(input_buffer, "\n")] = '\0';
    
    // ===================================================
    // FASE 4: Armazenar na Variável (auto-detecta tipo)
    // ===================================================
    
    const char* var_name = input_stmt->var_name;
    
    // Auto-detecta tipo
    if (strcmp(input_buffer, "true") == 0 || strcmp(input_buffer, "false") == 0)
    {
        // Boolean
        int bool_value = (strcmp(input_buffer, "true") == 0) ? 1 : 0;
        if (!symbol_table_set_bool(ctx->scope_stack->current_scope->symbols, var_name, bool_value))
        {
            printf("Evaluator error: failed to set boolean variable '%s'\n", var_name);
            return 0;
        }
    }
    else if (is_numeric_string(input_buffer))
    {
        // convert string para double
        // is_numeric_string() já verificou se a string representa um double
        errno = 0;
        double value = strtod(input_buffer, NULL);

        if (errno == ERANGE)
        {
            printf("Evaluator error: overflow in '%s'\n", input_buffer);
            return 0;
        }

        if (!symbol_table_set_number(ctx->scope_stack->current_scope->symbols, var_name, value))
        {
            printf("Evaluator error: failed to set number variable '%s'\n", var_name);
            return 0;
        }
    }
    else
    {
        // String
        if (!symbol_table_set_string(ctx->scope_stack->current_scope->symbols, var_name, input_buffer))
        {
            printf("Evaluator error: failed to set string variable '%s'\n", var_name);
            return 0;
        }
    }
    
    // ===================================================
    // FASE 5: Reset de Formatação e Cor
    // ===================================================
    
    if (input_stmt->set_nocolor == 1)
    {
        evaluator_color_reset(ctx);
    }
    
    reset_format(ctx);
    
    return 1;
}


// ==================================================================
// EVALUATE PRINT STATEMENT
// ==================================================================

// Função pública para avaliar print com contexto
static int evaluate_print_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    return evaluate_print_stmt_with_format(node, ctx);
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
        EvaluatorResult result = evaluate_expr(item_node, ctx, CTX_ANY);

        if (result.type == RESULT_ERROR)
        {
            //printf("%s\n", result.error_message);
            return 0;
        }
        
        // Converte para string
        char buffer[BUFFER_SIZE];
        if(result.type == RESULT_EMPTY)
        {
            snprintf(buffer, sizeof(buffer), "empty");
        }
        else if (result.type == RESULT_BOOL)
        {
            // Trata booleano
            snprintf(buffer, sizeof(buffer), "%s", 
                     result.value.boolean ? "true" : "false");
        }
        else if (result.type == RESULT_STRING)
        {
            snprintf(buffer, sizeof(buffer), "%s", result.value.string);
        }
        else if (result.type == RESULT_TEXT) 
        {
            // Text imprime o conteúdo diretamente
            const char* text_content = text_get(result.value.text);
            //snprintf(buffer, sizeof(buffer), "%s", text_content);
            printf("%s", text_content);  
            continue;
        }
        else if (result.type == RESULT_ARRAY)
        {
            // Imprime array no formato [elem1, elem2, elem3, ...]
            Array* arr = result.value.array;
            printf("[");
            int size = array_size(arr);
            for (int j = 0; j < size; j++)
            {
                if (j > 0) printf(", ");
                
                // Cada elemento é armazenado como void* que aponta para double*
                void* element = array_get(arr, j);
                if (element)
                {
                    if (arr->element_type == RESULT_NUMBER)
                    {
                        double value = *(double*)element;
                        if (fabs(value - (int)value) < EPSILON)
                        {
                            printf("%d", (int)value);
                        } else {
                            printf("%g", value);
                        }
                    }
                    else if (arr->element_type == RESULT_STRING)
                    {
                        printf("\"%s\"", (char*)element);
                    }
                    else if (arr->element_type == RESULT_TEXT)
                    {
                        const char* text_content = text_get((Text*)element);
                        printf("\"%s\"", text_content);
                    }
                }
                else
                {
                    printf("empty");
                }
            }
            printf("]");
            continue;
        }
        else // RESULT_NUMBER
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
// Fim de evaluate_print_stmt_with_format()

// ==================================================================
// FUNCOES PARA EXECUCAO (COM CONTEXTO)
// Executa statements (retorna sucesso/erro) let x = 10, print x, if ...
// ==================================================================
static int execute_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return 0;

    switch (node->type)
    {
        case NODE_ASSIGNMENT:
        {
            ASTNode* target_node = node->data.assignment.target;
            ASTNode* value_node = node->data.assignment.value;
            
            // Avalia o valor
            EvaluatorResult value_result = evaluate_expr(value_node, ctx, CTX_ANY);
            
            if (value_result.type == RESULT_ERROR)
            {
                has_evaluation_error = 1;
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            // CASO 1: Atribuição a variável simples (x = 10)
            if (target_node->type == NODE_VARIABLE)
            {
                const char* var_name = target_node->data.variable.var_name;
                
                // USA scope_set_value() QUE PROCURA NOS ESCOPOS PAI!
                if (!scope_set_value(ctx->scope_stack, var_name, value_result))
                {
                    printf("Evaluator error: variable '%s' not declared. Use 'let %s = value'\n",
                           var_name, var_name);
                    return 0;
                }
                
                return 1;
            }
            
            // CASO 2: Atribuição a elemento de array (arr[i] = x)
            else if (target_node->type == NODE_ARRAY_INDEX)
            {
                // Avalia o array (pode ser aninhado: arr[i][j])
                ASTNode* array_node = target_node->data.array_index.array;
                ASTNode* index_node = target_node->data.array_index.index;
                
                // Avalia o índice
                EvaluatorResult index_result = evaluate_expr(index_node, ctx, CTX_NUMBER);
                if (index_result.type == RESULT_ERROR)
                {
                    has_evaluation_error = 1;
                    printf("%s\n", index_result.error_message);
                    return 0;
                }
                
                int index = (int)index_result.value.number;
                
                // Se array_node é NODE_VARIABLE, é um array simples
                if (array_node->type == NODE_VARIABLE)
                {
                    const char* array_name = array_node->data.variable.var_name;
                    
                    // Procura o array nos escopos (atual → pai → global)
                    Array* array = NULL;
                    if (!scope_get_array(ctx->scope_stack, array_name, &array))
                    {
                        printf("Evaluator error: array '%s' not declared. Use 'let %s = array(size)'\n",
                               array_name, array_name);
                        return 0;
                    }
                    
                    // Encapsula o array em um EvaluatorResult
                    EvaluatorResult array_result = create_success_result_array(array, node->line, node->column);

                    // Verifica se o índice é válido (maior ou igual a zero)
                    // Para depois de zero não há problema porque o array é dinâmico
                    if (index < 0 )
                    {
                        printf("Evaluator error: array index out of bounds (index=%d; array size=%d\n", 
                               index, array->size);
                        return 0;
                    }
                    
                    // Atualiza o elemento do array
                    EvaluatorResult args[] = {array_result, index_result, value_result};

                    EvaluatorResult set_result = builtin_set(
                        args,
                        3,
                        node->line,
                        node->column,
                        ctx->scope_stack);

                    if (set_result.type == RESULT_ERROR)
                    {
                        printf("%s\n", set_result.error_message);
                        return 0;
                    }
                    
                    return 1;
                }
                
                // Se array_node é NODE_ARRAY_INDEX, é um array aninhado (arr[i][j])
                else if (array_node->type == NODE_ARRAY_INDEX)
                {
                    // Avalia recursivamente o array aninhado
                    EvaluatorResult array_result = evaluate_expr(array_node, ctx, CTX_ANY);
                    if (array_result.type == RESULT_ERROR)
                    {
                        has_evaluation_error = 1;
                        printf("%s\n", array_result.error_message);
                        return 0;
                    }
                    
                    Array* array = array_result.value.array;
                    
                    // Verifica se o índice é válido (maior ou igual a zero)
                    // Para depois de zero não há problema porque o array é dinâmico
                    if (index < 0)
                    {
                        printf("Evaluator error: array index out of bounds (index=%d; array size=%d)\n", 
                               index, array->size);
                        return 0;
                    }
                    
                    // Atualiza o elemento do array
                    EvaluatorResult args[] = {array_result, index_result, value_result};

                    EvaluatorResult set_result = builtin_set(
                        args,
                        3,
                        node->line,
                        node->column,
                        ctx->scope_stack);

                    if (set_result.type == RESULT_ERROR)
                    {
                        printf("%s\n", set_result.error_message);
                        return 0;
                    }
                                    
                    return 1;
                }
            }
            
            printf("Evaluator error: invalid assignment target\n");
            return 0;
        } // Fim do  case NODE_ASSIGNMENT:
            
        case NODE_BOOL:
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            // Avalia para exibição (qualquer tipo)
            EvaluatorResult result = evaluate_expr(node,
                                                   ctx,
                                                   CTX_ANY);
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
        } // Fim do case NODE_VARIABLE:
            
        case NODE_STRING:
        {
            // Standalone string
            printf("\"%s\"\n", node->data.string.value);
            return 1;
        }

        case NODE_PRINT:
        {
            return evaluate_print_stmt_with_ctx(node, ctx);
        }

        case NODE_COLOR:
            // Comando nocolor sozinho (ex: "nocolor" como statement)
            // Apenas reseta a cor
            if (node->data.color.color_token_id == 0) // CLR_NOCOLOR
            {  
                evaluator_color_reset(ctx);
                return 1;
            }
            // Outras cores sozinhas não fazem sentido como statements
            printf("%s[%d:%d] Evaluator warning: color command without print has no effect%s\n",
                   COLOR_WARNING, node->line, node->column, COLOR_RESET);
            return 1;

        case NODE_STATEMENT_LIST:
            return execute_stmt_list_with_ctx(node, ctx);

        case NODE_INPUT:
            return evaluate_input_stmt_with_ctx(node, ctx);

        case NODE_COMPARISON_OP:
        case NODE_LOGICAL_OP:
        case NODE_NOT_LOGICAL_OP:
        {
            // Avalia expressão lógica/de comparação
            EvaluatorResult result = evaluate_expr(node,
                                                   ctx,
                                                   CTX_ANY);
            
            if (result.type == RESULT_ERROR)
            {
                printf("%s\n", result.error_message);
                return 0;
            }
            else
            {
                // Exibe o resultado de acordo com o tipo
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
        } //Fim do case NODE_NOT_LOGICAL_OP:

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

        case NODE_DO:
            return execute_do_stmt_with_ctx(node, ctx);

        case NODE_FOR:
            return execute_for_stmt_with_ctx(node, ctx);

        case NODE_IMPORT:
        {
            ImportStatementData* import_data = &node->data.import_stmt;
            
            if (import_data->import_all)
            {
                // import math
                if (!module_manager_load(ctx->modules, import_data->module_name))
                {
                    printf("%sEvaluator error: failed to load module '%s'%s\n",
                           COLOR_ERROR, import_data->module_name, COLOR_RESET);
                    return 0;
                }
                printf("%sModule '%s' loaded successfully%s\n",
                       COLOR_SUCCESS, import_data->module_name, COLOR_RESET);
            }
            else
            {
                // from math import sqrt, pow, abs
                if (!module_manager_load(ctx->modules, import_data->module_name))
                {
                    printf("%sEvaluator error: failed to load module '%s'%s\n",
                           COLOR_ERROR, import_data->module_name, COLOR_RESET);
                    return 0;
                }
                
                // TODO: Importar funções específicas
                // Por enquanto, apenas carrega o módulo inteiro
                printf("%sModule '%s' loaded with selected functions."
                       "TODO: Importar funções específicas %s\n",
                       COLOR_SUCCESS, import_data->module_name, COLOR_RESET);
            }
            
            return 1;
        } // Fim do case NODE_IMPORT:

        case NODE_SAVE:  
            return execute_save_node(node, ctx);

        case NODE_FUNCTION_CALL:
        {
            // Avalia a chamada de função como statement (descarta resultado)
            EvaluatorResult result = evaluate_expr(node,
                                                   ctx,
                                                   CTX_ANY);
            
            if (result.type == RESULT_ERROR)
            {
                printf("%s\n", result.error_message);
                return 0;
            }
            
            // Função call como statement não imprime resultado
            // (diferente de expressão que seria impressa)
            return 1;
        }

        case NODE_LET:
        {
            ASTNode* target_node = node->data.let_stmt.target;
            ASTNode* value_node = node->data.let_stmt.value;
            
            // Avalia a expressão
            EvaluatorResult value_result = evaluate_expr(value_node, ctx, CTX_ANY);
            
            if (value_result.type == RESULT_ERROR)
            {
                has_evaluation_error = 1;
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            // Cria variável no escopo ATUAL (não procura nos escopos pai)
            SymbolTable* current_symbols = scope_get_symbols(ctx->scope_stack);
            if (!current_symbols)
            {
                printf("Evaluator error: no current scope\n");
                return 0;
            }
            
            // CASO 1: LET simples (let x = v)
            if (target_node->type == NODE_VARIABLE)
            {
                const char* var_name = target_node->data.variable.var_name;
                
                switch (value_result.type)
                {
                    case RESULT_NUMBER:
                        if (!symbol_table_set_number(current_symbols, var_name, value_result.value.number))
                        {
                            printf("Evaluator error: failed to create variable '%s'\n", var_name);
                            return 0;
                        }
                        break;
                    
                    case RESULT_STRING:
                        if (!symbol_table_set_string(current_symbols, var_name, value_result.value.string))
                        {
                            printf("Evaluator error: failed to create variable '%s'\n", var_name);
                            return 0;
                        }
                        break;
                    
                    case RESULT_BOOL:
                        if (!symbol_table_set_bool(current_symbols, var_name, value_result.value.boolean))
                        {
                            printf("Evaluator error: failed to create variable '%s'\n", var_name);
                            return 0;
                        }
                        break;
                    
                    case RESULT_TEXT:
                        if (!symbol_table_set_text(current_symbols, var_name, value_result.value.text))
                        {
                            printf("Evaluator error: failed to create variable '%s'\n", var_name);
                            return 0;
                        }
                        break;
                    
                    case RESULT_ARRAY:
                        if (!symbol_table_set_array(current_symbols, var_name, value_result.value.array))
                        {
                            printf("Evaluator error: failed to create variable '%s'\n", var_name);
                            return 0;
                        }
                        break;
                    
                    default:
                        printf("Evaluator error: invalid value type for variable '%s'\n", var_name);
                        return 0;
                }
                
                return 1;
            }
            
            // CASO 2: LET com array (let arr[i] = v)
            else if (target_node->type == NODE_ARRAY_INDEX)
            {
                // Avalia o array (pode ser aninhado: arr[i][j])
                ASTNode* array_node = target_node->data.array_index.array;
                ASTNode* index_node = target_node->data.array_index.index;
                
                // Avalia o índice
                EvaluatorResult index_result = evaluate_expr(index_node, ctx, CTX_NUMBER);
                if (index_result.type == RESULT_ERROR)
                {
                    has_evaluation_error = 1;
                    printf("%s\n", index_result.error_message);
                    return 0;
                }
                
                int index = (int)index_result.value.number;
                
                // Se array_node é NODE_VARIABLE, é um array simples
                if (array_node->type == NODE_VARIABLE)
                {
                    const char* array_name = array_node->data.variable.var_name;
                    
                    // Procura o array no escopo ATUAL (para LET, não procura nos escopos pai)
                    Array* array = NULL;
                    if (!symbol_table_get_array(current_symbols, array_name, &array))
                    {
                        printf("Evaluator error: array '%s' not found in current scope\n", array_name);
                        return 0;
                    }

                    // Encapsula o array em um EvaluatorResult
                    EvaluatorResult array_result = create_success_result_array(array, node->line, node->column);
                    
                    // Verifica se o índice é válido
                    // Não precisa verificar o limite positivo pois o array dinâmico
                    if (index < 0)
                    {
                        printf("Evaluator error: array index out of bounds (index=%d; array size=%d)\n", 
                               index, array->size);
                        return 0;
                    }
                    
                    // Atualiza o elemento do array
                    EvaluatorResult args[] = {array_result, index_result, value_result};

                    EvaluatorResult set_result = builtin_set(
                        args,
                        3,
                        node->line,
                        node->column,
                        ctx->scope_stack);

                    if (set_result.type == RESULT_ERROR)
                    {
                        printf("%s\n", set_result.error_message);
                        return 0;
                    }
                    
                    return 1;
                }
                
                // Se array_node é NODE_ARRAY_INDEX, é um array aninhado (arr[i][j])
                else if (array_node->type == NODE_ARRAY_INDEX)
                {
                    // Avalia recursivamente o array aninhado
                    EvaluatorResult array_result = evaluate_expr(array_node, ctx, CTX_ANY);
                    if (array_result.type == RESULT_ERROR)
                    {
                        has_evaluation_error = 1;
                        printf("%s\n", array_result.error_message);
                        return 0;
                    }
                    
                    Array* array = array_result.value.array;
                    
                    // Verifica se o índice é válido
                    // Não precisa verificar limite positivo pois o array é dinâmico
                    if (index < 0)
                    {
                        printf("Evaluator error: array index out of bounds (index=%d; array size=%d)\n", 
                               index, array->size);
                        return 0;
                    }
                    
                    // Atualiza o elemento do array
                    EvaluatorResult args[] = {array_result, index_result, value_result};

                    EvaluatorResult set_result = builtin_set(
                        args,
                        3,
                        node->line,
                        node->column,
                        ctx->scope_stack);

                    if (set_result.type == RESULT_ERROR)
                    {
                        printf("%s\n", set_result.error_message);
                        return 0;
                    }
                    
                    return 1;
                }
            }
            
            printf("Evaluator error: invalid LET target\n");
            return 0;
        } // Fim do case NODE_LET:

        case NODE_FUNCTION_DEF:
            return execute_function_def(node, ctx);

        case NODE_RETURN:
        {
            if (node->data.return_stmt.expression)
            {
                ctx->return_value = evaluate_expr(node->data.return_stmt.expression,
                                                   ctx,
                                                   CTX_ANY);
                if (ctx->return_value.type == RESULT_ERROR)
                {
                    return 0;
                }
            }
            else
            {
                ctx->return_value = create_success_result_empty(node->line, node->column);
            }
            
            ctx->should_return = 1;
            return 1;
        }
            
        default:
            printf("Evaluator error: unsupported statement type: %d\n", node->type);
            return 0;
    } // Fim do switch (node->type)

} // Fim de execute_stmt_with_ctx()

static int execute_stmt_list_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx || node->type != NODE_STATEMENT_LIST)
    {
        printf("Evaluator error: expected statement list node and context\n");
        return 0;
    }
    
    StatementListData* list = &node->data.stmt_list;
    
    // Executa cada statement em sequência
    for (int i = 0; i < list->count; i++)
    {
        ASTNode* stmt = list->statements[i];

        int success = execute_stmt_with_ctx(stmt, ctx);
        
        // Se deve sair do loop (break) ou pular (continue), ou retornar (return), para a execução
        if (ctx->should_break || ctx->should_continue || ctx->should_return)
        {
            break;
        }
    }
    
    return 1; 
} 
//Fim de execute_stmt_list_with_ctx() 

static int execute_if_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx) return 0;
    
    // Evaluate condition
    EvaluatorResult cond_result = evaluate_expr(
                                  node->data.if_stmt.condition,
                                  ctx, 
                                  CTX_BOOL);
    
    if (cond_result.type == RESULT_ERROR)
    {
        has_evaluation_error = 1;
        printf("%s\n", cond_result.error_message);
        return 0;
    }
    
    int result=  0;

    // Execute ramo apropriado
    if (cond_result.value.boolean)
    {
        // Cria novo escopo
        scope_push(ctx->scope_stack);

        // Executa ramo THEN 
        result = execute_stmt_with_ctx(node->data.if_stmt.then_body, ctx);

        // Libera escopo
        scope_pop(ctx->scope_stack);
    }
    else if (node->data.if_stmt.else_body)
    {
        // Executa ramo ELSE

        // Cria novo escopo
        scope_push(ctx->scope_stack);

        result = execute_stmt_with_ctx(node->data.if_stmt.else_body, ctx);

        // Libera escopo
        scope_pop(ctx->scope_stack);
    }
    else
    {
        result = 1; // Sem ramo ELSE, apenas retorna sucesso
    }
    
    return result;
}

static int execute_while_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx)
    {
        return 0;
    }
    
    ASTNode* condition = node->data.while_stmt.condition;
    ASTNode* body = node->data.while_stmt.body;

    // Cria novo escopo para o while
    scope_push(ctx->scope_stack);
    
    // Loop enquanto condição for verdadeira
    while (1)
    {
        // Avalia condição
        EvaluatorResult cond_result = evaluate_expr(condition,
                                                    ctx,
                                                    CTX_BOOL);
        
        if (cond_result.type == RESULT_ERROR)
        {
            has_evaluation_error = 1;
            printf("%s\n", cond_result.error_message);
            scope_pop(ctx->scope_stack);  // ← LIBERA ANTES DE SAIR!

            return 0;
        }
        
        // Se condição for falsa, sai do loop
        if (!cond_result.value.boolean)
        {
            break;
        }

        // Cria escopo para cada iteração
        scope_push(ctx->scope_stack);
        
        // Executa body
        int success = execute_stmt_with_ctx(body, ctx);

        // Libera escopo da iteração
        scope_pop(ctx->scope_stack);

        if (!success)
        {
            scope_pop(ctx->scope_stack);  // Libera escopo do WHILE
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
    
    scope_pop(ctx->scope_stack);  // Libera escopo do WHILE
    return 1;
}

//===================================================================
// Executa: do...end do (escopo) ou do...end while (loop)
//===================================================================
static int execute_do_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || !ctx)
    {
        return 0;
    }
    
    //ASTNode* condition = node->data.do_end_stmt.condition;
    ASTNode* body = node->data.do_end_stmt.body;

    // ===== CASO 1: do...end do (apenas escopo) =====
    if (!node->data.do_end_stmt.is_while)
    {
        // Cria novo escopo
        scope_push(ctx->scope_stack);
        
        // Executa body
        int success = execute_stmt_with_ctx(body, ctx);
        
        // Libera escopo
        scope_pop(ctx->scope_stack);
        
        return success;
    }
    
    // ===== CASO 2: do...end while (loop) =====
    else
    {
        
        // Cria novo escopo para o do...while
        scope_push(ctx->scope_stack);

        
        // Loop do...while (executa pelo menos 1 vez)
        do
        {

            // Cria escopo para cada iteração
            scope_push(ctx->scope_stack);
            
            // Executa body
            int success = execute_stmt_with_ctx(body, ctx);
            
            if (!success)
            {
                scope_pop(ctx->scope_stack);  // Libera escopo do DO...WHILE
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
                // Continua para a próxima iteração
            }
            
            ASTNode* condition = node->data.do_end_stmt.condition;
            
            // Se não foi continue, avalia condição
            // Avalia condição
            EvaluatorResult cond_result = evaluate_expr(condition,
                                                        ctx,
                                                        CTX_BOOL);
                

            if (cond_result.type == RESULT_ERROR)
            {
                has_evaluation_error = 1;
                printf("%s\n", cond_result.error_message);
                scope_pop(ctx->scope_stack);  // Libera escopo da iteração
                scope_pop(ctx->scope_stack);  // Libera escopo do DO...WHILE
                return 0;
            }

            // Se condição for falsa, sai do loop
            if (!cond_result.value.boolean)
            {
                scope_pop(ctx->scope_stack);  // Libera escopo da iteração
                break;
            }

            // Libera escopo da iteração para a próxima
            scope_pop(ctx->scope_stack); 

            printf("cond_result: %d\n", cond_result.value.boolean);

            // Se condição for falsa, sai do loop
            if (cond_result.value.boolean == 0)
            {
                break;
            }
            
        } while (1);
        
        scope_pop(ctx->scope_stack);  // Libera escopo do DO...WHILE
        return 1;
    }
}

static int execute_for_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || node->type != NODE_FOR || !ctx)
    {
        printf("Evaluator error: invalid for statement or context\n");
        return 0;
    }

    ForStatementData* for_stmt = &node->data.for_stmt;

    //SymbolTable* symbols = scope_get_symbols(ctx->scope_stack);
    
    // Avalia o valor inicial
    EvaluatorResult init_result = evaluate_expr(for_stmt->init_value,
                                                ctx,
                                                CTX_NUMBER);
    if (init_result.type == RESULT_ERROR)
    {
        has_evaluation_error = 1;
        printf("%s\n", init_result.error_message);
        return 0;
    }
    double init_value = init_result.value.number;

    // Avalia o valor final
    EvaluatorResult end_result = evaluate_expr(for_stmt->end_value,
                                               ctx,
                                               CTX_NUMBER);
    if (end_result.type == RESULT_ERROR)
    {
        has_evaluation_error = 1;
        printf("%s\n", end_result.error_message);
        return 0;
    }
    double end_value = end_result.value.number;

    // Avalia o step (padrão é 1)
    double step_value = 1.0;
    if (for_stmt->step_value)
    {
        EvaluatorResult step_result = evaluate_expr(for_stmt->step_value,
                                                    ctx,
                                                    CTX_NUMBER);
        if (step_result.type == RESULT_ERROR)
        {
            has_evaluation_error = 1;
            printf("%s\n", step_result.error_message);
            return 0;
        }
        step_value = step_result.value.number;
        
        // Verifica se step é zero (erro)
        if (fabs(step_value) < EPSILON)
        {
            printf("Evaluator error: step value cannot be zero in for loop\n");
            return 0;
        }
    }

    // Cria novo escopo para o for
    scope_push(ctx->scope_stack);    

    // Pega a tabela de símbolos do novo escopo
    SymbolTable* symbols = scope_get_symbols(ctx->scope_stack);

    // Executa o loop
    // Determina a direção do loop (crescente ou decrescente)
    int ascending = (step_value > 0);
    
    for (double i = init_value; 
         (ascending && i <= end_value + EPSILON) || (!ascending && i >= end_value - EPSILON);
         i += step_value)
    {
        if (!symbol_table_set_number(symbols,
                                     for_stmt->var_name,
                                     i))
        {
            printf("Evaluator error: failed to set loop variable '%s'\n", for_stmt->var_name);
            return 0;
        }

        // Cria novo escopo para cada iteração (para variáveis locais)
        scope_push(ctx->scope_stack);

        // Executa o corpo do loop
        int success = execute_stmt_list_with_ctx(for_stmt->body, ctx);

        // Ao sair do escopo libera variáveis locais da iteração
        scope_pop(ctx->scope_stack);  

        if (!success)
        {
            scope_pop(ctx->scope_stack);  // Libera escopo do for antes de retornar
            return 0;  
        }     

        // Verifica se deve sair do loop (break)
        if (ctx->should_break)
        {
            ctx->should_break = 0;  
            break;
        }

        // Verifica se deve pular para a próxima iteração
        if (ctx->should_continue)
        {
            ctx->should_continue = 0;
            continue;
        }
    }

    // Libera escopo do for
    scope_pop(ctx->scope_stack);
    return 1;  
}

static int execute_save_node(ASTNode* node, ExecutionContext* ctx)
{
    // Avalia a expressão que deve retornar um Text
    EvaluatorResult text_result = evaluate_expr(node->data.save_stmt.expression,
                                                ctx,
                                                CTX_ANY);
    
    if (text_result.type == RESULT_ERROR)
    {
        printf("%s\n", text_result.error_message);
        return 0;
    }
    
    // Verifica se é realmente um Text
    if (text_result.type != RESULT_TEXT)
    {
        printf("%s[%d:%d] Evaluator error: save() expects Text object, got %s%s\n",
               COLOR_ERROR, node->line, node->column,
               (text_result.type == RESULT_NUMBER ? "number" :
                text_result.type == RESULT_STRING ? "string" :
                text_result.type == RESULT_BOOL ? "boolean" : "unknown"),
               COLOR_RESET);
        return 0;
    }
    
    // Pega o nome do arquivo
    EvaluatorResult filename_result = evaluate_expr(
        node->data.save_stmt.filename_expr,
        ctx, CTX_ANY);  

    if (filename_result.type == RESULT_ERROR)
    {
        printf("%s%s%s\n", COLOR_ERROR, filename_result.error_message, COLOR_RESET);
        return 0;  
    }

    // Valida se é STRING
    if (filename_result.type != RESULT_STRING)
    {
        printf("%s[%d:%d] Evaluator error: save() expects string as filename, got %s%s\n",
               COLOR_ERROR, node->line, node->column,
               (filename_result.type == RESULT_NUMBER ? "number" :
                filename_result.type == RESULT_TEXT ? "text" :
                filename_result.type == RESULT_BOOL ? "boolean" : "unknown"),
               COLOR_RESET);
        return 0;
    }

    const char* filename = filename_result.value.string;
    
    // Salva o arquivo
    if (!text_save(text_result.value.text, filename))
    {
        printf("%s[%d:%d] Evaluator error: could not save file '%s'%s\n",
               COLOR_ERROR, node->line, node->column, filename, COLOR_RESET);
        return 0;
    }
    
    return 1;  // Sucesso
}


//===================================================================
// FUNCOES PARA GERENCIAMENTO DE MODULOS
//===================================================================
static ModuleManager* module_manager_create(void)
{
    ModuleManager* manager = A89ALLOC(sizeof(ModuleManager));
    
    // Alocar espaço inicial para 8 módulos
    manager->modules = A89ALLOC(sizeof(LoadedModule) * 8);
    
    manager->module_count = 0;
    manager->module_capacity = 8;
    
    // Inicializar array
    memset(manager->modules, 0, sizeof(LoadedModule) * 8);
    
    return manager;
}

static void module_manager_destroy(ModuleManager* manager)
{
    if (!manager) return;
    
    // Destruir todos os módulos carregados
    for (int i = 0; i < manager->module_count; i++)
    {
        if (manager->modules[i].symbols)
        {
            symbol_table_destroy(manager->modules[i].symbols);
        }
    }
    
    // Liberar array de módulos
    if (manager->modules)
    {
        a89free(manager->modules);
    }
    
    // Liberar manager
    a89free(manager);
}

static int module_manager_resize(ModuleManager* manager)
{
    if (!manager) return 0;
    
    // Calcula nova capacidade (1.5x) usando bit shift (>>)
    // Exemplo:
    // 8            = 1000
    // 8 >> 1       = 0100 (4)
    // 8 + (8 >> 1) = 12
    int new_capacity = manager->module_capacity + (manager->module_capacity >> 1);

    // CASO ESPECIAL: quando module_capacity = 1, 1.5× falha (dá 1) causando loop infinito
    // Garantimos pelo menos +1 elemento de crescimento
    if (new_capacity <= manager->module_capacity)
    {
        new_capacity = manager->module_capacity + 1;
    }
    
    // Alocar novo array
    LoadedModule* new_modules = A89ALLOC(sizeof(LoadedModule) * new_capacity);
    
    // Copiar dados existentes
    memcpy(new_modules, manager->modules, sizeof(LoadedModule) * manager->module_count);
    
    // Zerar a nova memória
    memset(&new_modules[manager->module_count], 0,
           sizeof(LoadedModule) * (new_capacity - manager->module_count));
    
    // Liberar array antigo
    a89free(manager->modules);
    
    // Atualizar manager
    manager->modules = new_modules;
    manager->module_capacity = new_capacity;
    
    return 1;
}


static int module_manager_load(ModuleManager* manager, const char* module_name)
{
    if (!manager || !module_name) return 0;
    
    // Verificar se módulo já está carregado
    for (int i = 0; i < manager->module_count; i++)
    {
        if (strcmp(manager->modules[i].name, module_name) == 0)
        {
            return 1;  // Já carregado
        }
    }
    
    // Verificar se precisa redimensionar
    if (manager->module_count >= manager->module_capacity)
    {
        if (!module_manager_resize(manager))
        {
            printf("%sEvaluator Error: Failed to load module '%s' (resize failed)%s\n",
                   COLOR_ERROR, module_name, COLOR_RESET);
            return 0;
        }
    }
    
    // Adicionar novo módulo
    // Pega o ENDEREÇO da próxima posição vazia no array
    LoadedModule* module = &manager->modules[manager->module_count];

    strncpy(module->name, module_name, VARNAME_SIZE - 1);
    module->name[VARNAME_SIZE - 1] = '\0';
    
    // Criar tabela de símbolos para o módulo
    module->symbols = symbol_table_create();
    manager->module_count++;
    return 1;
}

static SymbolTable* module_manager_get_symbols(ModuleManager* manager, 
                                               const char* module_name)
{
    if (!manager || !module_name) return NULL;
    
    for (int i = 0; i < manager->module_count; i++)
    {
        if (strcmp(manager->modules[i].name, module_name) == 0)
        {
            return manager->modules[i].symbols;
        }
    }
    
    return NULL;
}

//===================================================================
// EXECUTE FUNCTION DEFINITION
// Registra a função na symbol table do escopo global
//===================================================================
static int execute_function_def(ASTNode* node, ExecutionContext* ctx)
{
    if (!node || node->type != NODE_FUNCTION_DEF || !ctx)
    {
        printf("Evaluator error: expected function definition node\n");
        return 0;
    }
    
    FunctionDefData* func_data = &node->data.function_def;
    
    // Cria a estrutura UserFunction
    UserFunction* func = A89ALLOC(sizeof(UserFunction));
    
    // Copia o nome
    strncpy(func->name, func_data->function_name, VARNAME_SIZE - 1);
    func->name[VARNAME_SIZE - 1] = '\0';
    
    // Copia os parâmetros (apenas se houver)
    func->param_count = func_data->param_count;
    if (func_data->param_count > 0)
    {
        func->parameters = A89ALLOC(sizeof(char*) * func_data->param_count);
        for (int i = 0; i < func_data->param_count; i++)
        {
            func->parameters[i] = A89ALLOC(VARNAME_SIZE);
            strncpy(func->parameters[i], func_data->parameters[i], VARNAME_SIZE - 1);
            func->parameters[i][VARNAME_SIZE - 1] = '\0';
        }
    }
    else
    {
        func->parameters = NULL;
    }
    
    // Copia o corpo (AST)
    func->body = func_data->body;
    
    // Pega o nível de escopo atual (global)
    func->scope_level = ctx->scope_stack->scope_level;
    
    // Obtém a tabela de símbolos do escopo GLOBAL (nível 0)
    // Para isso, precisamos navegar até o escopo raiz
    Scope* global_scope = ctx->scope_stack->current_scope;
    while (global_scope->parent != NULL)
    {
        global_scope = global_scope->parent;
    }
    
    // Registra a função na tabela de símbolos global
    if (!symbol_table_set_function(global_scope->symbols, func->name, func))
    {
        printf("Evaluator error: failed to register function '%s'\n", func->name);
        
        // Limpa a memória alocada
        for (int i = 0; i < func->param_count; i++)
        {
            a89free(func->parameters[i]);
        }
        a89free(func->parameters);
        a89free(func);
        return 0;
    }
    
    return 1;
}



//###################################################################
// FUNCOES PUBLICAS
//###################################################################


// ==================================================================
// EVALUATE PROGRAM - FUNÇÃO PRINCIPAL PARA AVALIAR UM PROGRAMA COMPLETO
// ==================================================================
int evaluate_program(ASTNode* node)
{
    if (!node) return 0;
    
    // Cria contexto para a execução
    ExecutionContext* ctx = execution_ctx_create();
    if (!ctx) return 0;
    
    int result;
    if (node->type != NODE_STATEMENT_LIST)
    {
        result = execute_stmt_with_ctx(node, ctx);
    }
    else
    {
        result = execute_stmt_list_with_ctx(node, ctx);
    }
    
    execution_ctx_destroy(ctx);
    return result;
}


// ==================================================================
// EVALUATE EXPRESSIONS (COM CONTEXTO)
// Avalia expressões (retorna valor) x + 10, arr[i], func()
// ==================================================================
EvaluatorResult evaluate_expr(ASTNode* node, ExecutionContext* ctx, EvalContext eval_ctx)
{
    if (node == NULL)
    {
        return create_error_result("Evaluator error: AST node is null", 0, 0);
    }
    
    switch (node->type)

    {
        case NODE_EMPTY:
        {
            switch(eval_ctx)
            {
                case CTX_ANY:
                    return create_success_result_empty(node->line, node->column);
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: empty cannot be used as boolean");
                
                case CTX_NUMBER:
                    return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: empty cannot be used as number");
                
                case CTX_STRING:
                    return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: empty cannot be used as string");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: empty cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: unknown context");
            }
        }

        case NODE_BOOL:
        {
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_BOOL:
                    return create_success_result_bool(node->data.boolean.value, 
                                                       node->line, node->column);
                
                case CTX_NUMBER:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: boolean cannot be used as number");
                
                case CTX_STRING:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: boolean cannot be used as string");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: boolean cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unknown context");
            }
        }

        case NODE_NUMBER:
        {
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_NUMBER:
                    return create_success_result_number(node->data.number.value, 
                                                       node->line, node->column);
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: number cannot be used as boolean");
                
                case CTX_STRING:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: number cannot be used as string");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: number cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unknown context");
            }
        }
            
        case NODE_VARIABLE:
        {
            const char* var_name = node->data.variable.var_name;

            // Procura var_name em todos os escopos

            // Tenta como numero
            double num_value;
            if (scope_get_number(ctx->scope_stack, var_name, &num_value))
            {
                switch (eval_ctx)
                {
                    case CTX_ANY:
                    case CTX_NUMBER:
                        return create_success_result_number(num_value, node->line, node->column);
                    
                    case CTX_BOOL:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a number, cannot be used as boolean", 
                             var_name);
                    
                    case CTX_STRING:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a number, cannot be used as string", 
                             var_name);
                    
                    case CTX_ARRAY:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a number, cannot be used as array", 
                             var_name);
                    
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: unknown context");
                }
            }
            
            // Tenta como string
            char str_value[STRING_SIZE];
            if (scope_get_string(ctx->scope_stack, var_name, str_value, sizeof(str_value)))
            {
                switch (eval_ctx)
                {
                    case CTX_ANY:
                    case CTX_STRING:
                        return create_success_result_string(str_value, node->line, node->column);
                    
                    case CTX_NUMBER:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a string, cannot be used in mathematical operation", 
                             var_name);
                    
                    case CTX_BOOL:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a string, cannot be used as boolean", 
                             var_name);
                    
                    case CTX_ARRAY:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a string, cannot be used as array", 
                             var_name);
                    
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: unknown context");
                }
            }

            // Tenta como boolean
            int bool_value;
            if (scope_get_bool(ctx->scope_stack, var_name, &bool_value))
            {
                switch (eval_ctx)
                {
                    case CTX_ANY:
                    case CTX_BOOL:
                        return create_success_result_bool(bool_value, node->line, node->column);
                    
                    case CTX_NUMBER:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a boolean, cannot be used in mathematical operation", 
                             var_name);
                    
                    case CTX_STRING:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a boolean, cannot be used as string", 
                             var_name);
                    
                    case CTX_ARRAY:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is a boolean, cannot be used as array", 
                             var_name);
                    
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: unknown context");
                }
            }

            // Tenta como text
            Text* text_value;
            if (scope_get_text(ctx->scope_stack, var_name, &text_value))
            {
                switch (eval_ctx)
                {
                    case CTX_ANY:
                    case CTX_STRING:
                        return create_success_result_text(text_value, node->line, node->column);
                    
                    case CTX_NUMBER:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is Text, cannot be used as number", 
                             var_name);
                    
                    case CTX_BOOL:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is Text, cannot be used as boolean", 
                             var_name);
                    
                    case CTX_ARRAY:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is Text, cannot be used as array", 
                             var_name);
                    
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: unknown context");
                }
            }

            // Tenta como array
            Array* array_value;
            if (scope_get_array(ctx->scope_stack, var_name, &array_value))
            {
                switch (eval_ctx)
                {
                    case CTX_ANY:
                    case CTX_ARRAY:
                        return create_success_result_array(array_value, node->line, node->column);
                    
                    case CTX_NUMBER:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is an array, cannot be used as number", 
                             var_name);
                    
                    case CTX_BOOL:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is an array, cannot be used as boolean", 
                             var_name);
                    
                    case CTX_STRING:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: variable '%s' is an array, cannot be used as string", 
                             var_name);
                    
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: unknown context");
                }
            }
            // Não deve chegar aqui
            // Não encontrou em nenhum tipo
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: unknown '%s' variable", var_name);
        }
            
        case NODE_BINARY_OP:
        {

            char op = node->data.binaryop.operator;
            
            // ========================================
            // OPERADOR + (CONCATENAÇÃO OU SOMA)
            // ========================================
            if (op == '+')
            {
                // Avalia operandos SEM contexto específico (CTX_ANY)
                EvaluatorResult left_result = evaluate_expr(node->data.binaryop.left,
                                                            ctx,
                                                            CTX_ANY);
                
                if (left_result.type == RESULT_ERROR) return left_result;
                
                EvaluatorResult right_result = evaluate_expr(node->data.binaryop.right,
                                                             ctx,
                                                             CTX_ANY);
                
                if (right_result.type == RESULT_ERROR) return right_result;
                
                // ========================================
                // CASO 1: STRING + STRING
                // ========================================
                if (left_result.type == RESULT_STRING && right_result.type == RESULT_STRING)
                {
                    char result[STRING_SIZE];
                    snprintf(result, STRING_SIZE, "%s%s",
                             left_result.value.string,
                             right_result.value.string);
                    return create_success_result_string(result, node->line, node->column);
                }
                
                // ========================================
                // CASO 2: STRING + NUMBER
                // ========================================
                if (left_result.type == RESULT_STRING && right_result.type == RESULT_NUMBER)
                {
                    char result[STRING_SIZE];
                    snprintf(result, STRING_SIZE, "%s%g",
                             left_result.value.string,
                             right_result.value.number);
                    return create_success_result_string(result, node->line, node->column);
                }
                
                // ========================================
                // CASO 3: NUMBER + STRING
                // ========================================
                if (left_result.type == RESULT_NUMBER && right_result.type == RESULT_STRING)
                {
                    char result[STRING_SIZE];
                    snprintf(result, STRING_SIZE, "%g%s",
                             left_result.value.number,
                             right_result.value.string);
                    return create_success_result_string(result, node->line, node->column);
                }

                // ========================================
                // CASO 4: STRING + TEXT → TEXT
                // ========================================
                if (left_result.type == RESULT_STRING && right_result.type == RESULT_TEXT)
                {

                    size_t string_len = strlen(left_result.value.string);
                    size_t text_len = right_result.value.text->size;
                    size_t new_size = string_len + text_len + 1;

                    // Calcula o tamanho de string + Text.data
                    char* string_plus_text = A89ALLOC(new_size);

                    // Não há necessidade de testar o ponteiro alocado, A89ALLOC faz isso

                    // string + Text.dada
                    memcpy(string_plus_text, left_result.value.string, string_len);
                    memcpy(string_plus_text + string_len, right_result.value.text->data, text_len);
                    string_plus_text[new_size - 1] = '\0';  
                    
                    // Cria RESULT_TEXT para retornar
                    Text* result_text = text_create_from_string(string_plus_text);

                    // Libera o buffer temporário
                    a89free(string_plus_text);
                    
                    if (!result_text)
                    {
                        return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: text concatenation failed");
                    }

                    // Adiciona ao TextPool do escopo para gerenciar liberação
                    if (ctx && ctx->scope_stack)
                    {
                        scope_stack_add_text(ctx->scope_stack, result_text);  
                    }

                    
                    return create_success_result_text(result_text, node->line, node->column);
                }

                // ========================================
                // CASO 5: TEXT + STRING  → TEXT
                // ========================================
                if (left_result.type == RESULT_TEXT && right_result.type == RESULT_STRING)
                {

                    size_t text_len = left_result.value.text->size;
                    size_t string_len = strlen(right_result.value.string);
                    size_t new_size = text_len + string_len + 1;

                    // Calcula o tamanho de Text.data + string
                    char* text_plus_string = A89ALLOC(new_size);

                    // Não há necessidade de testar o ponteiro alocado, A89ALLOC faz isso

                    // Text.dada + string
                    memcpy(text_plus_string, left_result.value.text->data, text_len);
                    memcpy(text_plus_string + text_len, right_result.value.string, string_len);
                    text_plus_string[new_size - 1] = '\0';  
                    
                    // Cria RESULT_TEXT para retornar
                    Text* result_text = text_create_from_string(text_plus_string);

                    // Libera o buffer temporário
                    a89free(text_plus_string);
                    
                    if (!result_text)
                    {
                        return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: text concatenation failed");
                    }

                    // Adiciona ao TextPool do escopo para gerenciar liberação
                    if (ctx && ctx->scope_stack)
                    {
                        scope_stack_add_text(ctx->scope_stack, result_text);  // ou 'text'
                    }
                    
                    return create_success_result_text(result_text, node->line, node->column);
                }

                // ========================================
                // CASO 6: TEXT + TEXT  → TEXT
                // ========================================
                if (left_result.type == RESULT_TEXT && right_result.type == RESULT_TEXT)
                {

                    size_t text_left_len = left_result.value.text->size;
                    size_t text_right_len = right_result.value.text->size;
                    size_t new_size = text_left_len + text_right_len + 1;

                    // Calcula o tamanho de text_left + text_right
                    char* text_plus_text = A89ALLOC(new_size);

                    // Não há necessidade de testar o ponteiro alocado, A89ALLOC faz isso

                    // text_left.value.text->data + text_right.value.text->data
                    memcpy(text_plus_text, left_result.value.text->data, text_left_len);
                    memcpy(text_plus_text + text_left_len, right_result.value.text->data, text_right_len);
                    text_plus_text[new_size - 1] = '\0';  
                    
                    // Cria RESULT_TEXT para retornar
                    Text* result_text = text_create_from_string(text_plus_text);

                    // Libera o buffer temporário
                    a89free(text_plus_text);
                    
                    if (!result_text)
                    {
                        return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: text concatenation failed");
                    }
                    
                    // Adiciona ao TextPool do escopo para gerenciar liberação
                    if (ctx && ctx->scope_stack)
                    {
                        scope_stack_add_text(ctx->scope_stack, result_text);  // ou 'text'
                    }

                    return create_success_result_text(result_text, node->line, node->column);
                } 

                // ========================================
                // CASO 7: NUMBER + NUMBER (SOMA)
                // ========================================
                if (left_result.type == RESULT_NUMBER && right_result.type == RESULT_NUMBER)
                {
                    double result = left_result.value.number + right_result.value.number;
                    return create_success_result_number(result, node->line, node->column);
                }
                
                // ========================================
                // CASO 5: TIPOS NÃO SUPORTADOS
                // ========================================
                return create_error_result_fmt(node->line, node->column,
                    "Evaluator error: operator + not supported for %s and %s",
                    result_type_to_string(left_result.type),
                    result_type_to_string(right_result.type));
            }

            // Operações binárias sempre retornam NUMBER
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_NUMBER:
                    break;  // Continua com a avaliação
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: mathematical operation cannot be used as boolean");
                
                case CTX_STRING:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: mathematical operation cannot be used as string");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: mathematical operation cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unknown context");
            }

            // Avalia operandos no contexto CTX_NUMBER
            EvaluatorResult left_result = evaluate_expr(node->data.binaryop.left,
                                                        ctx,
                                                        CTX_NUMBER);

            if (left_result.type == RESULT_ERROR) return left_result;
            
            EvaluatorResult right_result = evaluate_expr(node->data.binaryop.right,
                                                         ctx,
                                                         CTX_NUMBER);

            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Verifica se ambos são NUMBERS (rejeita tudo mais)
            if (left_result.type != RESULT_NUMBER || right_result.type != RESULT_NUMBER)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: mathematical operation requires numbers, got %s and %s",
                     result_type_to_string(left_result.type),
                     result_type_to_string(right_result.type));
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
                case '%': 
                {
                    // Verifica divisão por zero
                    if (fabs(right_result.value.number) < EPSILON)
                    {
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: modulo by zero");
                    }
                    
                    // Truncated Division (mesmo sinal do dividendo)
                    long long left = (long long)left_result.value.number;
                    long long right = (long long)right_result.value.number;
                    result = (double)(left % right);
                    break;
                }

                default: 
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: invalid operator '%c'", node->data.binaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_UNARY_OP:
        {
            // Operação unária sempre retorna número
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_NUMBER:
                    // Aceita em CTX_ANY e CTX_NUMBER
                    break;  // Continua com a avaliação
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unary operator cannot be applied to boolean");
                
                case CTX_STRING:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unary operator cannot be applied to string");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unary operator cannot be applied to array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unknown context");
            }
            
            EvaluatorResult operand_result = evaluate_expr(node->data.unaryop.operand,
                                                           ctx,
                                                           CTX_NUMBER);

            if (operand_result.type == RESULT_ERROR)  return operand_result;
            
            if (operand_result.type != RESULT_NUMBER)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: unary operator requires number, got %s",
                     result_type_to_string(operand_result.type));
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
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_STRING:
                    return create_success_result_string(node->data.string.value, 
                                                       node->line, node->column);
                
                case CTX_NUMBER:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: string cannot be used as number");
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: string cannot be used as boolean");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: string cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Evaluator error: unknown context");
            }
        }

        case NODE_TEXT:
        {
            switch (eval_ctx)
            {
                case CTX_ANY:
                case CTX_STRING:
                {
                    // Converter NODE_TEXT para Text* (heap allocation)
                    Text* text = text_create_from_string(node->data.text.value);
                    if (!text)
                    {
                        return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: failed to create TEXT object");
                    }

                    // Adiciona ao TextPool do escopo para gerenciar liberação
                    if (ctx && ctx->scope_stack)
                    {
                        scope_stack_add_text(ctx->scope_stack, text);  // ou 'text'
                    }

                    return create_success_result_text(text, node->line, node->column);
                }
                
                case CTX_NUMBER:
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: text cannot be used as number");
                
                case CTX_BOOL:
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: text cannot be used as boolean");
                
                case CTX_ARRAY:
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: text cannot be used as array");
                
                default:
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: unknown context");
            }
        }


        case NODE_STATEMENT_LIST:
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: statement list cannot be used as expression");

        case NODE_COMPARISON_OP:
        {
            // Operações de comparação: ==, !=, <, >, <=, >=
            // Resultado é sempre booleano

            int comparison_result = 0;
            
            // Avalia lado esquerdo (pode ser número, booleano, string, text ou empty)
            EvaluatorResult left_result = evaluate_expr(node->data.logicalop.left,
                                                        ctx,
                                                        CTX_ANY);

            if (left_result.type == RESULT_ERROR) return left_result;
            
            // Avalia lado direito (pode ser número, booleano, string, text ou empty)
            EvaluatorResult right_result = evaluate_expr(node->data.logicalop.right,
                                                         ctx,
                                                         CTX_ANY);

            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Ambos devem ser do mesmo tipo (número, booleano ou empty)
            if (left_result.type != right_result.type)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: type mismatch in comparison: cannot compare %s with %s",
                     result_type_to_string(left_result.type),
                     result_type_to_string(right_result.type));
            }
            
            /* Comparação de string

            Detalhes Importantes
            
            strcmp() retorna:
                Negativo se left < right
                Zero se left == right
                Positivo se left > right

            Ordem lexicográfica: strcmp() já faz isso automaticamente

            Case-sensitive: strcmp() é case-sensitive (como desejado)*/
            if (left_result.type == RESULT_STRING)
            {
                const char* left = left_result.value.string;
                const char* right = right_result.value.string;
                
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result = string_equals(left, right) ? 1 : 0;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = string_equals(left, right) ? 0 : 1;
                        break;
                    case OP_LESS:  
                        comparison_result = (strcmp(left, right) < 0) ? 1 : 0;
                        break;
                    case OP_GREATER:  
                        comparison_result = (strcmp(left, right) > 0) ? 1 : 0;
                        break;
                    case OP_LESS_EQUAL: 
                        comparison_result = (strcmp(left, right) <= 0) ? 1 : 0;
                        break;
                    case OP_GREATER_EQUAL:  
                        comparison_result = (strcmp(left, right) >= 0) ? 1 : 0;
                        break;
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: operator not supported for strings");
                }
                return create_success_result_bool(comparison_result, 
                                                  node->line, node->column);
            }

            // Comparação de text
            if (left_result.type == RESULT_TEXT)
            {
                const char* left = left_result.value.text->data;
                const char* right = right_result.value.text->data;
                
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result = string_equals(left, right) ? 1 : 0;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = string_equals(left, right) ? 0 : 1;
                        break;
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: operator not supported for Text");
                }
                return create_success_result_bool(comparison_result, 
                                                  node->line, node->column);
            }
                
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
                return create_success_result_bool(comparison_result, 
                                                  node->line, node->column);
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
                return create_success_result_bool(comparison_result, 
                                                  node->line, node->column);
            }

            // Comparação de array
            else if (left_result.type == RESULT_ARRAY)
            {
                Array* left = left_result.value.array;
                Array* right = right_result.value.array;
                
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result =  array_equals(left, right) ? 1 : 0;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = array_equals(left, right) ? 0 : 1;
                        break;
                    // Outros operadores não fazem sentido para arrays
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: operator not supported for arrays");
                }
                return create_success_result_bool(comparison_result, 
                                                  node->line, node->column);
            }

            // Comparação de empty; se chegar aqui já se sabe que são dois empty
            else 
            {
                switch (node->data.logicalop.operator)
                {
                    case OP_EQUAL:
                        comparison_result = 1;
                        break;
                    case OP_NOT_EQUAL:
                        comparison_result = 0;
                        break;
                    // Outros operadores não fazem sentido para empty
                    default:
                        return create_error_result_fmt(node->line, node->column,
                             "Evaluator error: operator not supported for empty values");
                }
            }
            
            //return create_success_result_bool(comparison_result, node->line, node->column);
        }

        case NODE_LOGICAL_OP:
        {
            // Operações lógicas: AND, OR
            // Resultado é sempre booleano
            
            LogicalOperator op = node->data.logicalop.operator;
            
            // Avalia lado esquerdo (deve ser booleano)
            EvaluatorResult left_result = evaluate_expr(node->data.logicalop.left,
                                                        ctx,
                                                        CTX_BOOL);

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
            EvaluatorResult right_result = evaluate_expr(node->data.logicalop.right,
                                                         ctx,
                                                         CTX_BOOL);

            if (right_result.type == RESULT_ERROR) return right_result;
            
            // Verifica se é booleano
            if (right_result.type != RESULT_BOOL)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: logical operator expects boolean, got %s",
                        result_type_to_string(right_result.type));
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
            EvaluatorResult operand_result = evaluate_expr(node->data.notop.operand,
                                                           ctx,
                                                           CTX_BOOL);

            if (operand_result.type == RESULT_ERROR) return operand_result;
            
            // Verifica se é booleano
            if (operand_result.type != RESULT_BOOL)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Evaluator error: NOT operator expects boolean, got %s",
                        result_type_to_string(operand_result.type));
            }
            
            int operand = operand_result.value.boolean;
            int not_result = !operand;  // Inverte o valor
            
            return create_success_result_bool(not_result, 
                                             node->line, node->column);
        }

        case NODE_FUNCTION_CALL:
        {
            // Avalia a chamada de função
            const char* func_name = node->data.function_call.function_name;
            
            // ============================================
            // FUNÇÃO: array(size)
            // ============================================
            if (strcmp(func_name, "array") == 0)
            {
                if (node->data.function_call.arg_count != 1)
                {
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: array() expects 1 argument, got %d",
                        node->data.function_call.arg_count);
                }
                
                // Avalia o argumento como número
                EvaluatorResult arg_result = evaluate_expr(node->data.function_call.arguments[0],
                                                           ctx,
                                                           CTX_NUMBER);
                
                if (arg_result.type == RESULT_ERROR) return arg_result;
                
                if (arg_result.type != RESULT_NUMBER)
                {
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: array() expects number argument");
                }
                
                // Cria array com capacidade solicitada
                int requested_capacity = (int)arg_result.value.number;
                Array* array = array_create_with_capacity(requested_capacity);
                if (!array)
                {
                    return create_error_result_fmt(node->line, node->column,
                        "Evaluator error: could not create array");
                }
                
                return create_success_result_array(array, node->line, node->column);
            }

            // ============================================
            // DISPATCH PARA FUNÇÕES BUILT-IN
            // ============================================
            for (int i = 0; builtins[i].name != NULL; i++)
            {
                if (strcmp(func_name, builtins[i].name) == 0)
                {
                    // Avalia argumentos
                    EvaluatorResult* args = A89ALLOC(sizeof(EvaluatorResult) * node->data.function_call.arg_count);
                    
                    for (int j = 0; j < node->data.function_call.arg_count; j++)
                    {
                        args[j] = evaluate_expr(node->data.function_call.arguments[j],
                                                ctx,
                                                CTX_ANY);
                        
                        if (args[j].type == RESULT_ERROR)
                        {
                            a89free(args);
                            return args[j];  // Retorna o erro
                        }
                    }
                    
                    // Chama a função através do ponteiro
                    EvaluatorResult result = builtins[i].func(
                        args,
                        node->data.function_call.arg_count,
                        node->line,
                        node->column,
                        ctx->scope_stack 
                    );
                    
                    a89free(args);
                    return result;
                }
            }

            // ============================================
            // FUNÇÃO DEFINIDA PELO USUÁRIO
            // ============================================
            
            // Procura a função na tabela de símbolos (escopo global)
            // Primeiro, encontra o escopo global
            Scope* global_scope = ctx->scope_stack->current_scope;
            while (global_scope->parent != NULL)
            {
                global_scope = global_scope->parent;
            }
            
            UserFunction* func = NULL;
            if (!symbol_table_get_function(global_scope->symbols, func_name, &func))
            {
                return create_error_result_fmt(node->line, node->column,
                    "Evaluator error: unknown function '%s'", func_name);
            }
            
            // Verifica número de argumentos
            if (node->data.function_call.arg_count != func->param_count)
            {
                return create_error_result_fmt(node->line, node->column,
                    "Evaluator error: function '%s' expects %d arguments, got %d",
                    func_name, func->param_count, node->data.function_call.arg_count);
            }
            
            // Avalia os argumentos (apenas se houver parâmetros)
            EvaluatorResult* arg_values = NULL;
            if (func->param_count > 0)
            {
                arg_values = A89ALLOC(sizeof(EvaluatorResult) * func->param_count);
                for (int i = 0; i < func->param_count; i++)
                {
                    arg_values[i] = evaluate_expr(node->data.function_call.arguments[i],
                                                  ctx,
                                                  CTX_ANY);
                    if (arg_values[i].type == RESULT_ERROR)
                    {
                        a89free(arg_values);
                        return arg_values[i];
                    }
                }
            }
            
            // Cria novo escopo para a função
            scope_push(ctx->scope_stack);
            SymbolTable* local_symbols = scope_get_symbols(ctx->scope_stack);
            
            // Liga os parâmetros aos argumentos
            for (int i = 0; i < func->param_count; i++)
            {
                const char* param_name = func->parameters[i];
                EvaluatorResult arg = arg_values[i];
                
                switch (arg.type)
                {
                    case RESULT_NUMBER:
                        symbol_table_set_number(local_symbols, param_name, arg.value.number);
                        break;
                    case RESULT_STRING:
                        symbol_table_set_string(local_symbols, param_name, arg.value.string);
                        break;
                    case RESULT_BOOL:
                        symbol_table_set_bool(local_symbols, param_name, arg.value.boolean);
                        break;
                    case RESULT_TEXT:
                        symbol_table_set_text(local_symbols, param_name, arg.value.text);
                        break;
                    case RESULT_ARRAY:
                        symbol_table_set_array(local_symbols, param_name, arg.value.array);
                        break;
                    default:
                        a89free(arg_values);
                        scope_pop(ctx->scope_stack);
                        return create_error_result_fmt(node->line, node->column,
                            "Evaluator error: unsupported argument type for parameter '%s'",
                            param_name);
                }
            }
            
            a89free(arg_values);
            
            // Reseta as flags de return antes de executar
            ctx->should_return = 0;
            ctx->return_value.type = RESULT_EMPTY;
            
            // Executa o corpo da função
            int success = execute_stmt_list_with_ctx(func->body, ctx);
            
            // Verifica se houve return
            EvaluatorResult result;
            if (ctx->should_return && ctx->return_value.type != RESULT_EMPTY)
            {
                result = ctx->return_value;
            }
            else if (ctx->should_return && ctx->return_value.type == RESULT_EMPTY)
            {
                // return sem expressão
                result = create_success_result_empty(node->line, node->column);
            }
            else
            {
                // Sem return, retorna empty
                result = create_success_result_empty(node->line, node->column);
            }
            
            // Limpa as flags
            ctx->should_return = 0;
            ctx->return_value.type = RESULT_EMPTY;
            
            // Libera o escopo da função
            scope_pop(ctx->scope_stack);
            
            if (!success)
            {
                return create_error_result_fmt(node->line, node->column,
                    "Evaluator error: function '%s' execution failed", func_name);
            }
            
            return result;
        }

        case NODE_ARRAY_INDEX:
        {
            // Avalia o array
            EvaluatorResult arr_result = evaluate_expr(node->data.array_index.array,
                                                       ctx,
                                                       CTX_ANY);
            
            if (arr_result.type == RESULT_ERROR)
            {
                return arr_result;
            }
            
            // Avalia o índice
            EvaluatorResult index_result = evaluate_expr(node->data.array_index.index,
                                                         ctx,
                                                         CTX_ANY);
            
            if (index_result.type == RESULT_ERROR)
            {
                return index_result;
            }
            
            // Chama get()
            EvaluatorResult args[] = {arr_result, index_result};
            return builtin_get(
                        args,
                        2,
                        node->line,
                        node->column,
                        ctx->scope_stack);
        }      
            
        default:
            return create_error_result_fmt(node->line, node->column,
                 "Evaluator error: unsupported node type: %d", node->type);
    }
} // Fim de evaluate_expr()


//===================================================================
// FUNCOES PARA GERENCIAR CONTEXTO
//===================================================================
ExecutionContext* execution_ctx_create(void)
{
    ExecutionContext* ctx = A89ALLOC(sizeof(ExecutionContext));
    ctx->scope_stack = scope_stack_create();
    ctx->current_color = "";
    ctx->color_enabled = 1;
    ctx->should_break = 0;      
    ctx->should_continue = 0;
    ctx->should_return = 0;           
    ctx->return_value.type = RESULT_EMPTY;  
    ctx->modules = module_manager_create();   
    reset_format(ctx);    
    return ctx;
}

void execution_ctx_destroy(ExecutionContext* ctx)
{
    if (!ctx) return;
    
    if (ctx->modules)
    {
        module_manager_destroy(ctx->modules); 
        ctx->modules = NULL;
    }

    if (ctx->scope_stack)
    {
        scope_stack_destroy(ctx->scope_stack);
    }
    
    a89free(ctx);
}

void evaluator_reset_format(ExecutionContext* ctx)
{
    reset_format(ctx);
}


//###################################################################
// FUNÇÕES DE GERENCIAMENTO DE CORES COM CONTEXTO
//###################################################################

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
    if (!ctx)
    {
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

// Fim de evaluator.c
