// zztext_wrapper.h

#ifndef ZZTEXT_WRAPPER_H
#define ZZTEXT_WRAPPER_H

#include "result.h"
#include "scope.h"

// ============================================================
// WRAPPERS DE TEXT
// ============================================================

// text(string) - Cria novo Text object
EvaluatorResult builtin_text(
    EvaluatorResult* args, 
    int arg_count, 
    int line, 
    int column,
    ScopeStack* scope_stack);

// load(filename) - Carrega arquivo em Text object
EvaluatorResult builtin_load(
    EvaluatorResult* args, 
    int arg_count, 
    int line, 
    int column,
    ScopeStack* scope_stack);

// save(text, filename) - Salva Text object em arquivo
EvaluatorResult builtin_save(
    EvaluatorResult* args, 
    int arg_count, 
    int line, 
    int column,
    ScopeStack* scope_stack);

#endif
// Fim de zztext_wrapper.h
