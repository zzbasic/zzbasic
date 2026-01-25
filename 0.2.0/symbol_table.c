// symbol_table.c

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zzdefs.h"
#include "color.h"
#include "symbol_table.h"
#include "a89alloc.h"

// ============================================
// PRIVATE DEFINITIONS
// ============================================

typedef enum
{
    SYM_NUMBER,
    SYM_STRING
} SymbolType;

typedef struct Symbol
{
    char name[VARNAME_SIZE];
    SymbolType type;
    union
    {
        double num_value;
        char str_value[STRING_SIZE];
    } value;
    struct Symbol* next;
} Symbol;

struct SymbolTable
{
    Symbol* head;
    int count;
};

// ============================================
// PRIVATE HELPER FUNCTIONS (static)
// ============================================

static Symbol* find_symbol(SymbolTable* table, const char* name)
{
    if (!table || !name) return NULL;
    
    Symbol* current = table->head;
    while (current)
    {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ============================================
// PUBLIC INTERFACE IMPLEMENTATION
// ============================================

SymbolTable* symbol_table_create(void)
{
    SymbolTable* table = A89ALLOC(sizeof(SymbolTable));
    if (!table)
    {
        fprintf(stderr, "%sError: could not create symbol table%s\n",
                COLOR_ERROR, COLOR_RESET);
        return NULL;
    }
    
    table->head = NULL;
    table->count = 0;
    return table;
}

void symbol_table_destroy(SymbolTable* table)
{
    if (!table) return;
    
    Symbol* current = table->head;
    while (current)
    {
        Symbol* next = current->next;
        a89free(current);
        current = next;
    }
    
    a89free(table);
}

int symbol_table_set_number(SymbolTable* table, const char* name, double value)
{
    if (!table || !name) return 0;
    
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len >= VARNAME_SIZE)
    {
        fprintf(stderr, "%sError: invalid variable name: '%.*s...'%s\n", 
                COLOR_ERROR, VARNAME_SIZE - 4, name, COLOR_RESET);
        return 0;
    }
    
    Symbol* symbol = find_symbol(table, name);
    
    if (!symbol)
    {
        // Create new symbol
        symbol = A89ALLOC(sizeof(Symbol));
        if (!symbol)
        {
            fprintf(stderr, "%sError: insufficient memory for variable '%s'%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        
        strncpy(symbol->name, name, VARNAME_SIZE - 1);
        symbol->name[VARNAME_SIZE - 1] = '\0';
        symbol->type = SYM_NUMBER;
        symbol->value.num_value = value;
        
        // Insert at beginning
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    }
    else
    {
        // Update existing
        if (symbol->type != SYM_NUMBER)
        {
            fprintf(stderr, "%sError: variable '%s' is a string, not a number%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        symbol->value.num_value = value;
    }
    
    return 1;
}

int symbol_table_set_string(SymbolTable* table, const char* name, const char* value)
{
    if (!table || !name || !value) return 0;
    
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len >= VARNAME_SIZE)
    {
        fprintf(stderr, "%sError: invalid variable name: '%.*s...'%s\n", 
                COLOR_ERROR, VARNAME_SIZE - 4, name, COLOR_RESET);
        return 0;
    }
    
    Symbol* symbol = find_symbol(table, name);
    
    if (!symbol)
    {
        // Create new symbol
        symbol = A89ALLOC(sizeof(Symbol));
        if (!symbol)
        {
            fprintf(stderr, "%sError: insufficient memory for variable '%s'%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        
        strncpy(symbol->name, name, VARNAME_SIZE - 1);
        symbol->name[VARNAME_SIZE - 1] = '\0';
        symbol->type = SYM_STRING;
        strncpy(symbol->value.str_value, value, STRING_SIZE - 1);
        symbol->value.str_value[STRING_SIZE - 1] = '\0';
        
        // Insert at beginning
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    }
    else
    {
        // Update existing
        if (symbol->type != SYM_STRING)
        {
            fprintf(stderr, "%sError: variable '%s' is a number, not a string%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        strncpy(symbol->value.str_value, value, STRING_SIZE - 1);
        symbol->value.str_value[STRING_SIZE - 1] = '\0';
    }
    
    return 1;
}

int symbol_table_get_number(SymbolTable* table, const char* name, double* out_value)
{
    if (!table || !name || !out_value) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol)
    {
        return 0;  // Variable doesn't exist
    }
    
    if (symbol->type != SYM_NUMBER)
    {
        return 0;  // It's a string, not a number
    }
    
    *out_value = symbol->value.num_value;
    return 1;
}

int symbol_table_get_string(SymbolTable* table, const char* name, char* out_value, size_t max_len)
{
    if (!table || !name || !out_value || max_len == 0) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol)
    {
        return 0;  // Variable doesn't exist
    }
    
    if (symbol->type != SYM_STRING)
    {
        return 0;  // It's a number, not a string
    }
    
    strncpy(out_value, symbol->value.str_value, max_len - 1);
    out_value[max_len - 1] = '\0';
    return 1;
}

int symbol_table_exists(SymbolTable* table, const char* name)
{
    if (!table || !name) return 0;
    return find_symbol(table, name) != NULL;
}

int symbol_table_count(SymbolTable* table)
{
    return table ? table->count : 0;
}

void symbol_table_print(SymbolTable* table)
{
    if (!table)
    {
        printf("Symbol table: NULL\n");
        return;
    }
    
    printf("%s=== SYMBOL TABLE (%d variables) ===%s\n", 
            COLOR_HEADER, table->count, COLOR_RESET);
    
    Symbol* current = table->head;
    while (current)
    {
        printf("  %s: ", current->name);
        
        if (current->type == SYM_NUMBER)
        {
            printf("[NUM] %.6g", current->value.num_value);
        }
        else
        {
            printf("[STR] \"%s\"", current->value.str_value);
        }
        
        printf("\n");
        current = current->next;
    }
    
    if (table->count == 0)
    {
        printf("  (empty)\n");
    }
    
    printf("===================================\n");
}

// ============================================
// QUICK TEST
// ============================================

#ifdef TESTSYMBOLTABLE
#include "utils.h"

int main() {
    setup_utf8();
    printf("%s=== Teste Tabela de Símbolos (v0.2.0 completo) ===%s\n\n",
            COLOR_HEADER, COLOR_RESET);
    
    // 1. Creation
    SymbolTable* table = symbol_table_create();
    if (!table) {
        printf("%sERRO: Não foi possível criar tabela%s\n", COLOR_ERROR, COLOR_RESET);
        return 1;
    }
    
    printf("1. Tabela criada (count=%d)\n", symbol_table_count(table));
    
    // 2. Number insertion
    printf("\n2. Inserindo variáveis numéricas:\n");
    
    if (symbol_table_set_number(table, "x", 10.5)) {
        printf("   x = 10.5 (inserido)\n");
    } else {
        printf("   %sERRO ao inserir x%s\n", COLOR_ERROR, COLOR_RESET);
    }
    
    if (symbol_table_set_number(table, "y", 20.0)) {
        printf("   y = 20.0 (inserido)\n");
    }
    
    if (symbol_table_set_number(table, "total", 30.5)) {
        printf("   total = 30.5 (inserido)\n");
    }
    
    // 3. String insertion
    printf("\n3. Inserindo variáveis string:\n");
    
    if (symbol_table_set_string(table, "nome", "João")) {
        printf("   nome = \"João\" (inserido)\n");
    }
    
    if (symbol_table_set_string(table, "mensagem", "Olá Mundo!")) {
        printf("   mensagem = \"Olá Mundo!\" (inserido)\n");
    }
    
    // 4. Update
    printf("\n4. Atualizando variável existente:\n");
    if (symbol_table_set_number(table, "x", 15.0)) {
        printf("   x atualizado para 15.0\n");
    }
    
    if (symbol_table_set_string(table, "nome", "Maria")) {
        printf("   nome atualizado para \"Maria\"\n");
    }
    
    // 5. Query
    printf("\n5. Consultando valores:\n");
    
    double num_val;
    if (symbol_table_get_number(table, "x", &num_val)) {
        printf("   x = %.2f\n", num_val);
    } else {
        printf("   %sErro ao ler x%s\n", COLOR_ERROR, COLOR_RESET);
    }
    
    if (symbol_table_get_number(table, "y", &num_val)) {
        printf("   y = %.2f\n", num_val);
    }
    
    char str_val[100];
    if (symbol_table_get_string(table, "nome", str_val, sizeof(str_val))) {
        printf("   nome = \"%s\"\n", str_val);
    } else {
        printf("   %sErro ao ler nome%s\n", COLOR_ERROR, COLOR_RESET);
    }
    
    // 6. Expected errors
    printf("\n6. Testando erros:\n");
    
    // Variable doesn't exist
    if (!symbol_table_get_number(table, "z", &num_val)) {
        printf("   ✓ Variável 'z' não existe (comportamento esperado)\n");
    }
    
    // Wrong type
    if (!symbol_table_get_number(table, "nome", &num_val)) {
        printf("   ✓ 'nome' é string, não número (comportamento esperado)\n");
    }
    
    if (!symbol_table_get_string(table, "x", str_val, sizeof(str_val))) {
        printf("   ✓ 'x' é número, não string (comportamento esperado)\n");
    }
    
    // 7. Existence check
    printf("\n7. Verificando existência:\n");
    printf("   'x' existe? %s\n", symbol_table_exists(table, "x") ? "SIM" : "NÃO");
    printf("   'nome' existe? %s\n", symbol_table_exists(table, "nome") ? "SIM" : "NÃO");
    printf("   'inexistente' existe? %s\n", symbol_table_exists(table, "inexistente") ? "SIM" : "NÃO");
    
    // 8. Complete print
    printf("\n8. Tabela completa:\n");
    symbol_table_print(table);
    
    // 9. Cleanup
    printf("\n9. Destruindo tabela...\n");
    symbol_table_destroy(table);
    printf("   Tabela destruída\n");
    
    printf("\n%s=== Teste concluído ===%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    a89check_leaks();
    return 0;
}
#endif