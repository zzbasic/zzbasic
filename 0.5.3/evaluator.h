// evaluator.h

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "zzdefs.h"
#include "ast.h"
#include "symbol_table.h"
#include "color_mapping.h"


// =================================================
// Tipos de alinhamento
// =================================================
typedef enum {
    ALIGN_LEFT = 0,
    ALIGN_RIGHT,
    ALIGN_CENTER
} AlignmentType;

// =================================================
// Estrutura para formato de saída
// =================================================
typedef struct {
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
    CTX_STRING    // Espera string (concatenação futura: "Olá " + nome)
} EvalContext;

typedef enum
{
    RESULT_ERROR,
    RESULT_BOOL,
    RESULT_NUMBER,
    RESULT_STRING
} ResultType;

// =================================================
// Contexto de execução - print e while usam
// =================================================
typedef struct {
    SymbolTable* symbols;        // Tabela de símbolos
    const char* current_color;   // Cor atual aplicada
    int color_enabled;           // Se cores estão habilitadas
    OutputFormat format;         // Formatação (width e alignment)
    int should_break;      
    int should_continue;   
} ExecutionContext;

typedef struct
{
    ResultType type;
    union {
        int boolean;
        double number;
        char string[STRING_SIZE];
    } value;
    char error_message[BUFFER_SIZE];
    int line;   
    int column; 
} EvaluatorResult;


// width e alignment
ExecutionContext* execution_ctx_create(SymbolTable* symbols);
void execution_ctx_destroy(ExecutionContext* ctx);
void evaluator_reset_format(ExecutionContext* ctx);// Formatação

// Funções de gerenciamento de cores
void evaluator_color_reset(ExecutionContext* ctx);
void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color);
void evaluator_color_apply_current(ExecutionContext* ctx);


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
    

********************************************************************/
int evaluate_program(ASTNode* node, SymbolTable* symbols);

int execute_stmt_list(ASTNode* node, SymbolTable* symbols);
int execute_stmt(ASTNode* node, SymbolTable* symbols);
int execute_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

EvaluatorResult evaluate_expr(ASTNode* node, SymbolTable* symbols, EvalContext ctx);

int evaluate_print_stmt(ASTNode* node, SymbolTable* symbols);
int evaluate_print_with_ctx(ASTNode* node, ExecutionContext* ctx);

//int evaluate_print_statement_with_context(ASTNode* node, ExecutionContext* ctx);

int evaluate_input_stmt(ASTNode* node, SymbolTable* symbols);

int execute_if_stmt(ASTNode* node, SymbolTable* symbols);
int execute_if_stmt_with_ctx(ASTNode* node, ExecutionContext* ctx);

// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node);

#endif
// Fim de evaluator.h