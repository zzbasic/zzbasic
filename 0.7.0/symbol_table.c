// symbol_table.c

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zzdefs.h"
#include "color.h"
#include "symbol_table.h"
#include "a89alloc.h"

typedef enum
{
    SYM_NUMBER,
    SYM_STRING,
    SYM_BOOL,
    SYM_TEXT,
    SYM_ARRAY
} SymbolType;

typedef struct Symbol
{
    char name[VARNAME_SIZE];
    SymbolType type;
    union
    {
        int         bool_value;
        double      num_value;
        char        str_value[STRING_SIZE];
        Text*       text_value;
        Array*      array_value;
    } value;
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable
{
    Symbol* head;
    int count;
} SymbolTable;

// ============================================
// PRIVATE HELPER FUNCTIONS (static)
// ============================================

Symbol* find_symbol(SymbolTable* table, const char* name)
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

        // Libera text se existir
        if (current->type == SYM_TEXT && current->value.text_value)
        {
            //a89free(current->value.text_value);
            text_free(current->value.text_value);
        }

        if (current->type == SYM_ARRAY && current->value.array_value)
        {
            array_free(current->value.array_value);
        }

        a89free(current);
        current = next;
    }
    
    a89free(table);
}


static int is_valid_name(const char* name)
{
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len >= VARNAME_SIZE)
    {
        fprintf(stderr, "%sError: invalid variable name: '%.*s...'%s\n", 
                COLOR_ERROR, VARNAME_SIZE - 4, name, COLOR_RESET);
        return 0;
    }
    return 1;
}

int symbol_table_set_bool(SymbolTable* table, const char* name, int value)
{
    if (!table || !name || !is_valid_name(name)) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    
    if (!symbol)
    {
        // Create new symbol
        symbol = A89ALLOC(sizeof(Symbol));
        strncpy(symbol->name, name, VARNAME_SIZE - 1);
        symbol->name[VARNAME_SIZE - 1] = '\0';
        symbol->type = SYM_BOOL;
        symbol->value.bool_value = value;
        
        // Insert at beginning
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    }
    else
    {
        // Update existing
        if (symbol->type != SYM_BOOL)
        {
            fprintf(stderr, "%sError: variable '%s' is not a boolean%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        symbol->value.bool_value = value;
    }
    
    return 1;   
}

int symbol_table_get_bool(SymbolTable* table, const char* name, int* out_value)
{
    if (!table || !name || !out_value) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol)
    {
        return 0;  // Variable doesn't exist
    }
    
    if (symbol->type != SYM_BOOL)
    {
        return 0;  // It's not a boolean
    }
    
    *out_value = symbol->value.bool_value;
    return 1;    
}

int symbol_table_set_number(SymbolTable* table, const char* name, double value)
{
    if (!table || !name || !is_valid_name(name)) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    
    if (!symbol)
    {
        // Create new symbol
        symbol = A89ALLOC(sizeof(Symbol));
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
            fprintf(stderr, "%sError: variable '%s' is not a number%s\n",
                    COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        symbol->value.num_value = value;
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
        return 0;  // It's not a number
    }
    
    *out_value = symbol->value.num_value;
    return 1;
}

int symbol_table_set_string(SymbolTable* table, const char* name, const char* value)
{
    if (!table || !name || !is_valid_name(name)) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    
    if (!symbol)
    {
        // Create new symbol
        symbol = A89ALLOC(sizeof(Symbol));
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
            //fprintf(stderr, "%sError: variable '%s' is not a string%s\n",
            //        COLOR_ERROR, name, COLOR_RESET);
            return 0;
        }
        strncpy(symbol->value.str_value, value, STRING_SIZE - 1);
        symbol->value.str_value[STRING_SIZE - 1] = '\0';
    }
    
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
        return 0;  // It's not a string
    }
    
    strncpy(out_value, symbol->value.str_value, max_len - 1);
    out_value[max_len - 1] = '\0';
    return 1;
}

int symbol_table_set_text(SymbolTable* table, const char* name, Text* text)
{
    if (!table || !name || !is_valid_name(name)) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    
    if (symbol)
    {
        // Libera texto antigo se existir
        if (symbol->type == SYM_TEXT && symbol->value.text_value)
        {
            text_free(symbol->value.text_value);  // ← usa text_free()!
        }
        
        symbol->type = SYM_TEXT;
        symbol->value.text_value = text;  // ← só copia o ponteiro!
        return 1;
    }
    
    // Criar novo símbolo
    symbol = A89ALLOC(sizeof(Symbol));
    strncpy(symbol->name, name, VARNAME_SIZE - 1);
    symbol->name[VARNAME_SIZE - 1] = '\0';
    symbol->type = SYM_TEXT;
    symbol->value.text_value = text;  // ← só copia o ponteiro!
    
    symbol->next = table->head;
    table->head = symbol;
    table->count++;
    
    return 1;
}

int symbol_table_get_text(SymbolTable* table, const char* name, Text** out_text)
{
    if (!table || !name || !out_text) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol || symbol->type != SYM_TEXT) return 0;
    
    *out_text = (Text*)symbol->value.text_value;  // ← copia o ponteiro Text*
    return 1;
}

int symbol_table_set_array(SymbolTable* table, const char* name, Array* array)
{
    if (!table || !name || !array) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol)
    {
        symbol = A89ALLOC(sizeof(Symbol));
        
        strncpy(symbol->name, name, VARNAME_SIZE - 1);
        symbol->name[VARNAME_SIZE - 1] = '\0';
        symbol->type = SYM_ARRAY;
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    }
    else
    {
        // Libera array anterior se existir
        if (symbol->type == SYM_ARRAY && symbol->value.array_value)
        {
            array_free(symbol->value.array_value);
        }
        symbol->type = SYM_ARRAY;
    }
    
    symbol->value.array_value = array;
    return 1;
}

int symbol_table_get_array(SymbolTable* table, const char* name, Array** array)
{
    if (!table || !name || !array) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    if (!symbol || symbol->type != SYM_ARRAY) return 0;
    
    *array = symbol->value.array_value;
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
        
        switch(current->type)
        {
            case SYM_BOOL:
                printf("[BOOL] %s", current->value.bool_value ? "true" : "false");
                break;
            case SYM_NUMBER:
                printf("[NUM] %.6g", current->value.num_value);
                break;
            case SYM_STRING:
                printf("[STR] \"%s\"", current->value.str_value);
                break;

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

// Fim de symbol_table.c