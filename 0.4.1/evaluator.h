// evaluator.h

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "zzdefs.h"
#include "ast.h"
#include "symbol_table.h"
#include "color_mapping.h"

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
// Contexto de execução que mantém estado das cores
// =================================================
typedef struct {
    SymbolTable* symbols;      // Tabela de símbolos
    const char* current_color; // Cor atual aplicada (ANSI string)
    int color_enabled;         // Se cores estão habilitadas
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
EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx);

// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node);

#endif
