// scope.c

#include <string.h>

#include "scope.h"
#include "a89alloc.h"

ScopeStack* scope_stack_create(void)
{
    ScopeStack* scope_stack = A89ALLOC(sizeof(ScopeStack));
    
    scope_stack->scope_level = 0;
    scope_stack->current_scope = A89ALLOC(sizeof(Scope));
    
    scope_stack->current_scope->scope_level = 0;
    scope_stack->current_scope->parent = NULL;
    scope_stack->current_scope->symbols = symbol_table_create();
    
    return scope_stack;
}

void scope_stack_destroy(ScopeStack* scope_stack)
{
    if (!scope_stack) return;
    
    Scope* scope = scope_stack->current_scope;
    while (scope)
    {
        Scope* parent = scope->parent;
        if (scope->symbols)
        {
            symbol_table_destroy(scope->symbols);
        }
        a89free(scope);
        scope = parent;
    }
    
    a89free(scope_stack);
}

void scope_push(ScopeStack* scope_stack)
{
    if (!scope_stack) return;
    
    Scope* new_scope = A89ALLOC(sizeof(Scope));
    
    // Incrementa o nível da pilha de escopos
    scope_stack->scope_level++;
    
    // Depois configura o novo escopo da pilha 
    new_scope->scope_level = scope_stack->scope_level;
    new_scope->parent = scope_stack->current_scope;
    new_scope->symbols = symbol_table_create();
    
    if (!new_scope->symbols)
    {
        a89free(new_scope);
        scope_stack->scope_level--;  // Decrementa o nivel da pilha de escopos
        return;
    }
    
    scope_stack->current_scope = new_scope;
}


void scope_pop(ScopeStack* scope_stack)
{
    if (!scope_stack || !scope_stack->current_scope || !scope_stack->current_scope->parent)
    {
        return;  // Não pode sair do escopo global
    }
    
    Scope* old_scope = scope_stack->current_scope;
    scope_stack->current_scope = old_scope->parent;
    scope_stack->scope_level--;
    
    // Libera símbolos do escopo que está saindo
    if (old_scope->symbols)
    {
        symbol_table_destroy(old_scope->symbols);
    }
    a89free(old_scope);
}

Scope* scope_get_current(ScopeStack* scope_stack)
{
    return scope_stack ? scope_stack->current_scope : NULL;
}

SymbolTable* scope_get_symbols(ScopeStack* scope_stack)
{
    Scope* scope = scope_get_current(scope_stack);
    return scope ? scope->symbols : NULL;
}

Symbol* scope_lookup(ScopeStack* scope_stack, const char* name)
{
    if (!scope_stack || !name) return NULL;
    
    Scope* scope = scope_stack->current_scope;
    
    // Procura do escopo atual até o global
    while (scope)
    {
        Symbol* sym = find_symbol(scope->symbols, name);
        if (sym) return sym;
        scope = scope->parent;
    }
    
    return NULL;
}

int scope_set_value(ScopeStack* stack, const char* name, EvaluatorResult result)
{
    if (!stack || !name) return 0;
    
    SymbolTable* symbols = scope_get_symbols(stack);
    if (!symbols) return 0;
    
    // Chama a função SET correta baseado no tipo
    switch (result.type) {
        case RESULT_BOOL:
            return symbol_table_set_bool(symbols, name, result.value.boolean);
        case RESULT_NUMBER:
            return symbol_table_set_number(symbols, name, result.value.number);
        case RESULT_STRING:
            return symbol_table_set_string(symbols, name, result.value.string);
        case RESULT_TEXT:
            return symbol_table_set_text(symbols, name, result.value.text);
        case RESULT_ARRAY:
            return symbol_table_set_array(symbols, name, result.value.array);
        default:
            return 0;
    }
}

// Fim de scope.c