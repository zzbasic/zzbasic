// zzstring_wrapper.h

#ifndef ZZSTRING_WRAPPER_H
#define ZZSTRING_WRAPPER_H

#include "result.h"

// ============================================================
// WRAPPERS DE STRING
// ============================================================

// len(string) - Retorna tamanho da string
EvaluatorResult builtin_len_string(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// substr(string, start, length) - Extrai substring
EvaluatorResult builtin_substr(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// upper(string) - Converte para maiúsculas
EvaluatorResult builtin_upper(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// lower(string) - Converte para minúsculas
EvaluatorResult builtin_lower(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// trim(string) - Remove espaços em branco
EvaluatorResult builtin_trim(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// split(string, delimiter) - Divide em array
EvaluatorResult builtin_split(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

// join(array, delimiter) - Junta array em string
EvaluatorResult builtin_join(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack);

#endif
// Fim de zzstring_wrapper.h