// symbol_table.h

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>  // Para size_t

#include "zztext.h"
#include "zzarray.h"

// Tipo opaco (encapsulamento)
typedef struct SymbolTable SymbolTable;
typedef struct Symbol Symbol;


Symbol* find_symbol(SymbolTable* table, const char* name);

// Criação/destruição
SymbolTable* symbol_table_create(void);
void symbol_table_destroy(SymbolTable* table);

// Operações Bool
int symbol_table_set_bool(SymbolTable* table, const char* name, int value);
int symbol_table_get_bool(SymbolTable* table, const char* name, int* out_value);

// Operações numéricas
int symbol_table_set_number(SymbolTable* table, const char* name, double value);
int symbol_table_get_number(SymbolTable* table, const char* name, double* out_value);

// Operações com strings
int symbol_table_set_string(SymbolTable* table, const char* name, const char* value);
int symbol_table_get_string(SymbolTable* table, const char* name, char* out_value, size_t max_len);

// Operações com text (strings dinâmicas)
int symbol_table_set_text(SymbolTable* table, const char* name, Text* text);
int symbol_table_get_text(SymbolTable* table, const char* name, Text** out_text);

int symbol_table_set_array(SymbolTable* table, const char* name, Array* array);
int symbol_table_get_array(SymbolTable* table, const char* name, Array** array);


// Consultas
int symbol_table_exists(SymbolTable* table, const char* name);  // 1=existe, 0=não existe
int symbol_table_count(SymbolTable* table);  // número de variáveis

// Debug
void symbol_table_print(SymbolTable* table);

#endif
// Fim de symbol_table.h