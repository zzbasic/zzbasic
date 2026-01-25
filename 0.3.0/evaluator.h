#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "zzdefs.h"
#include "ast.h"
#include "symbol_table.h"

// ============================================
// Contexto de avaliação (para verificação de tipos)
// ============================================
typedef enum
{
    CTX_ANY,      // Qualquer tipo aceitável (para exibição: PRINT x)
    CTX_NUMBER,   // Espera número (operações matemáticas: x + 5)
    CTX_STRING    // Espera string (concatenação futura: "Olá " + nome)
} EvalContext;

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


int execute_statement_list(ASTNode* node, SymbolTable* symbols);

EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx);

int execute_statement(ASTNode* node, SymbolTable* symbols);

int evaluate_program(ASTNode* node, SymbolTable* symbols);

// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node);

#endif
