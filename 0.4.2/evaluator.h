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
    CTX_NUMBER,   // Espera número (operações matemáticas: x + 5)
    CTX_STRING    // Espera string (concatenação futura: "Olá " + nome)
} EvalContext;


// =================================================
// Contexto de execução 
// =================================================
typedef struct {
    SymbolTable* symbols;        // Tabela de símbolos
    const char* current_color;   // Cor atual aplicada
    int color_enabled;           // Se cores estão habilitadas
    OutputFormat format;         // Formatação (width e alignment)
} ExecutionContext;

typedef struct
{
    int success;    // 1 = sucesso, 0 = erro
    int is_string;  // 1 = string, 0 = number
    union {
        double number;
        char string[STRING_SIZE];
    } value;
    char error_message[BUFFER_SIZE];
    int line;   
    int column; 
} EvaluatorResult;


// width e alignment
ExecutionContext* execution_context_create(SymbolTable* symbols);
void execution_context_destroy(ExecutionContext* ctx);
void evaluator_reset_format(ExecutionContext* ctx);// Formatação

// Funções de gerenciamento de cores
void evaluator_color_reset(ExecutionContext* ctx);
void evaluator_color_set(ExecutionContext* ctx, const char* ansi_color);
void evaluator_color_apply_current(ExecutionContext* ctx);

// Funções principais
int evaluate_program(ASTNode* node, SymbolTable* symbols);
int execute_statement_list(ASTNode* node, SymbolTable* symbols);
int execute_statement(ASTNode* node, SymbolTable* symbols);

// Versão com contexto de execução (para cores)
int execute_statement_with_context(ASTNode* node, ExecutionContext* ctx);
int evaluate_print_statement_with_context(ASTNode* node, ExecutionContext* ctx);

int evaluate_print_statement(ASTNode* node, SymbolTable* symbols);
int evaluate_print_with_context(ASTNode* node, ExecutionContext* ctx);
EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx);
int execute_statement_with_context(ASTNode* node, ExecutionContext* ctx);
void evaluator_color_apply_current(ExecutionContext* ctx);



// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node);

#endif
// Fim de evaluator.h