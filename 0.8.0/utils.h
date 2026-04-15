// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "result.h"
#include "scope.h"

void setup_utf8();

void zzwait();

/********************************************************************
Exibe uma linha de código-fonte com uma seta apontando para o erro

Parâmetros:
	* source      Ponteiro para o código-fonte completo
	* source_size Tamanho total do código-fonte em bytes
	* line        Número da linha (1-indexado)
	* column      Número da coluna (1-indexado)
 
Exemplo de saída:
	Linha 5: let x = 10 + 
                      ↑
*********************************************************************/
void display_error_location(const char* source, int source_size, int line, int column);


//===================================================================
// len() - Genérico para Array, Text e String
// Identifica o tipo e chama a função específica
//===================================================================
EvaluatorResult builtin_len(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);


#endif
// Fim de utils.h