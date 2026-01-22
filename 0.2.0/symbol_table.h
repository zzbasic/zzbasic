#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// ============================================
// Tabela de Símbolos - ZzBasic v0.2.0
// ============================================

// Tipo opaco (encapsulamento)
typedef struct SymbolTable SymbolTable;

// Criação/destruição
SymbolTable* symbol_table_create(void);
void symbol_table_destroy(SymbolTable* table);

// Operações numéricas (v0.2.0 - APENAS números)
int symbol_table_put(SymbolTable* table, const char* name, double value);  // 1=sucesso, 0=erro
int symbol_table_get(SymbolTable* table, const char* name, double* out_value);  // 1=sucesso, 0=erro

// Consultas
int symbol_table_exists(SymbolTable* table, const char* name);  // 1=existe, 0=não existe
int symbol_table_count(SymbolTable* table);  // número de variáveis

// Debug
void symbol_table_print(SymbolTable* table);

// ============================================
// FUNÇÕES PARA VERSÕES FUTURAS (COMENTADAS)
// ============================================

/*
// Para v0.3.0 ou v0.4.0 (strings)
int symbol_table_put_string(SymbolTable* table, const char* name, const char* value);
int symbol_table_get_string(SymbolTable* table, const char* name, char* out_value, size_t max_len);

// Para v0.5.0 (tipagem dinâmica)
typedef enum {
    SYM_NUMBER,
    SYM_STRING,
    SYM_BOOLEAN  // futuro
} SymbolType;

int symbol_table_get_type(SymbolTable* table, const char* name, SymbolType* out_type);
*/

#endif