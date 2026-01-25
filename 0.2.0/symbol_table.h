#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>  // Para size_t

// ============================================
// Tabela de Símbolos - ZzBasic v0.2.0
// ============================================

// Tipo opaco (encapsulamento)
typedef struct SymbolTable SymbolTable;

// Criação/destruição
SymbolTable* symbol_table_create(void);
void symbol_table_destroy(SymbolTable* table);

// Operações numéricas
int symbol_table_set_number(SymbolTable* table, const char* name, double value);
int symbol_table_get_number(SymbolTable* table, const char* name, double* out_value);

// Operações com strings
int symbol_table_set_string(SymbolTable* table, const char* name, const char* value);
int symbol_table_get_string(SymbolTable* table, const char* name, char* out_value, size_t max_len);

// Consultas
int symbol_table_exists(SymbolTable* table, const char* name);  // 1=existe, 0=não existe
int symbol_table_count(SymbolTable* table);  // número de variáveis

// Debug
void symbol_table_print(SymbolTable* table);

#endif