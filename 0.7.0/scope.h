// scope.h

#ifndef SCOPE_H
#define SCOPE_H

#include "result.h"
#include "symbol_table.h"

typedef struct Scope {
    int scope_level;           // Nível do escopo (0=global, 1+=local)
    SymbolTable* symbols;      // Símbolos deste escopo
    struct Scope* parent;      // Escopo pai (NULL se global)
} Scope;

typedef struct {
    Scope* current_scope;      // Escopo atual
    int scope_level;           // Contador de profundidade
} ScopeStack;

ScopeStack* scope_stack_create(void);
void scope_stack_destroy(ScopeStack* scope_stack);
void scope_push(ScopeStack* scope_stack);
void scope_pop(ScopeStack* scope_stack);
Scope* scope_get_current(ScopeStack* scope_stack);
SymbolTable* scope_get_symbols(ScopeStack* scope_stack);
Symbol* scope_lookup(ScopeStack* scope_stack, const char* name);
int scope_set_value(ScopeStack* scope_stack, const char* name, EvaluatorResult value);

#endif
// Fim de scope.h