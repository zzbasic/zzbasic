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

int symbol_exists(SymbolTable* table, const char* name);

ScopeStack* scope_stack_create(void);
void scope_stack_destroy(ScopeStack* scope_stack);
void scope_push(ScopeStack* scope_stack);
void scope_pop(ScopeStack* scope_stack);
Scope* scope_get_current(ScopeStack* scope_stack);
SymbolTable* scope_get_symbols(ScopeStack* scope_stack);
Symbol* scope_lookup(ScopeStack* scope_stack, const char* name);
int scope_set_value(ScopeStack* scope_stack, const char* name, EvaluatorResult value);

// Funções de leitura que fazem lookup em todos os escopos
int scope_get_number(ScopeStack* stack, const char* name, double* out_value);
int scope_get_string(ScopeStack* stack, const char* name, char* out_value, size_t max_len);
int scope_get_bool(ScopeStack* stack, const char* name, int* out_value);
int scope_get_text(ScopeStack* stack, const char* name, Text** out_text);
int scope_get_array(ScopeStack* stack, const char* name, Array** out_array);

#endif
// Fim de scope.h