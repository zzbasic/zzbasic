// evaluator.h

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "zzdefs.h"
#include "ast.h"
#include "symbol_table.h"
#include "color_mapping.h"
#include "zztext.h"
#include "result.h"
#include "scope.h"

// =================================================
// Tipos de alinhamento
// =================================================
typedef enum
{
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER
} AlignmentType;

// =================================================
// Estrutura para formato de saída
// =================================================
typedef struct
{
    int width;           // Largura do campo (0 = sem formatação)
    AlignmentType align; // Tipo de alinhamento
    int has_format;      // 1 = formato ativo, 0 = formato padrão
} OutputFormat;


// =================================================
// Contexto de avaliação (para verificação de tipos)
// =================================================
typedef enum
{
    CTX_ANY,      // Qualquer tipo aceitável (para exibição: PRINT x)
    CTX_BOOL,     // Espera um booleano: true(1) ou false(0)
    CTX_NUMBER,   // Espera número (operações matemáticas: x + 5)
    CTX_STRING,   // Espera string (concatenação futura: "Olá " + nome)
    CTX_ARRAY
} EvalContext;

// =================================================
// Estrutura para gerenciar módulos carregados
// =================================================
typedef struct
{
    char name[VARNAME_SIZE];           // Nome do módulo (ex: "math")
    SymbolTable* symbols;              // Símbolos do módulo
    // Futuro: funções, constantes, etc.
} LoadedModule;

typedef struct {
    LoadedModule* modules;          // Alocação dinamica.
    int module_count;                  // Número de módulos carregados
    int module_capacity;               // Inicia com 8, cresce 1.5x
} ModuleManager;

// =================================================
// Contexto de execução - print e while usam
// =================================================
typedef struct
{
    ScopeStack* scope_stack;        // Pilha de escopos
    const char* current_color;      // Cor atual aplicada
    int color_enabled;              // Se cores estão habilitadas
    OutputFormat format;            // Formatação (width e alignment)
    int should_break;      
    int should_continue; 
    ModuleManager* modules; 

    int should_return;              // Flag indicando que deve retornar da função
    EvaluatorResult return_value;   // Valor a ser retornado (se should_return for 1)
} ExecutionContext;


/********************************************************************
FLUXO DO EVALUATOR

evaluate_program() ==> execute_stmt_list() ==> execute_stmt()

execute_stmt():
    NODE_ASSIGNMENT ==> evaluate_expr()

    NODE_BOOL:
    NODE_NUMBER:
    NODE_BINARY_OP:
    NODE_UNARY_OP:
    NODE_VARIABLE: ==> evaluate_expr()

    NODE_STRING ==> evaluate_print_stmt()

    NODE_STATEMENT_LIST ==> execute_stmt_list()

    NODE_INPUT ==> evaluate_input_stmt()

    NODE_COMPARISON_OP
    NODE_LOGICAL_OP
    NODE_NOT_LOGICAL_OP ==> evaluate_expr()

    NODE_IF ==> execute_if_stmt()

    NODE_WHILE ==> execution_ctx_create()
                   execute_while_stmt_with_ctx()
                   execution_ctx_destroy(ctx)

    NODE_BREAK ==> return 0;

    NODE_CONTINUE ==> return 0;

    default ==> Evaluator error: unsupported statement type
                return 0;

********************************************************************/
int evaluate_program(ASTNode* node);

EvaluatorResult evaluate_expr(ASTNode* node, ExecutionContext* ctx, EvalContext eval_ctx);

// width e alignment
ExecutionContext* execution_ctx_create(void);
void execution_ctx_destroy(ExecutionContext* ctx);
void evaluator_reset_format(ExecutionContext* ctx);// Formatação

// Funções de gerenciamento de cores
void evaluator_color_reset(ExecutionContext* ctx);
void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color);
void evaluator_color_apply_current(ExecutionContext* ctx);

void debug_alloc_counter(ASTNode* node, const char* operation);

#endif
// Fim de evaluator.h