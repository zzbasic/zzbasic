#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "utils.h"
#include "ast.h"

typedef struct {
    int success;    // 1 = sucesso, 0 = erro
    int is_string;  // 1 = string, 0 = number;// SERÁ USADO COM STRING 
    union {
        double number_value;
        char string_value[BUFFER_SIZE];// SERÁ USADO COM STRING
    } value;
    char error_message[BUFFER_SIZE];
    int line;   // Linha do node
    int column; // Coluna do node
} EvaluatorResult;

// Avalia uma AST e retorna o resultado
EvaluatorResult evaluate(ASTNode* node);

#endif
