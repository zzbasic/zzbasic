#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "color.h"
#include "utils.h"
#include "symbol_table.h"
#include "a89alloc.h"  

// ============================================
// DEFINIÇÕES PRIVADAS - v0.2.0
// ============================================

#define MAX_VAR_NAME 64      // 63 caracteres + \0 (igual lexer.h)
#define MAX_STR_VALUE 256    // Para versões futuras (strings)

typedef enum {
    SYM_NUMBER  // APENAS números na v0.2.0
} SymbolType;

typedef struct Symbol {
    char name[MAX_VAR_NAME];
    SymbolType type;
    union {
        double num_value;
        char str_value[MAX_STR_VALUE];
    } value;
    struct Symbol* next;
} Symbol;

struct SymbolTable {
    Symbol* head;
    int count;
};

// ============================================
// FUNÇÕES AUXILIARES PRIVADAS (static)
// ============================================

static Symbol* find_symbol(SymbolTable* table, const char* name) {
    if (!table || !name) return NULL;
    
    Symbol* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ============================================
// IMPLEMENTAÇÃO DA INTERFACE PÚBLICA (v0.2.0)
// ============================================

SymbolTable* symbol_table_create(void) {
    SymbolTable* table = A89ALLOC(sizeof(SymbolTable));
    if (table == NULL) {
        fprintf(stderr, "Erro: não foi possível criar tabela de símbolos\n");
        return NULL;
    }
    
    table->head = NULL;
    table->count = 0;
    return table;
}

void symbol_table_destroy(SymbolTable* table) {
    if (table == NULL) return;
    
    Symbol* current = table->head;
    while (current != NULL) {
        Symbol* next = current->next;
        a89free(current);
        current = next;
    }
    
    a89free(table);
}

int symbol_table_put(SymbolTable* table, const char* name, double value) {
    if (table == NULL || name == NULL) return 0;  // 0 = erro
    
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len >= MAX_VAR_NAME) {
        fprintf(stderr, "Erro: nome de variável inválido: '%.*s...'\n", 
                MAX_VAR_NAME - 4, name);
        return 0;
    }
    
    Symbol* symbol = find_symbol(table, name);
    
    if (symbol == NULL) {
        symbol = A89ALLOC(sizeof(Symbol));
        if (symbol == NULL) {
            fprintf(stderr, "Erro: memória insuficiente para variável '%s'\n", name);
            return 0;
        }
        
        strncpy(symbol->name, name, MAX_VAR_NAME - 1);
        symbol->name[MAX_VAR_NAME - 1] = '\0';
        
        symbol->type = SYM_NUMBER;
        symbol->value.num_value = value;
        
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    } else {
        if (symbol->type != SYM_NUMBER) {
            fprintf(stderr, "Erro: variável '%s' não é numérica\n", name);
            return 0;
        }
        symbol->value.num_value = value;
    }
    
    return 1;  // 1 = sucesso
}

int symbol_table_get(SymbolTable* table, const char* name, double* out_value) {
    if (table == NULL || name == NULL || out_value == NULL) {
        return 0;  // Erro: parâmetros inválidos
    }
    
    Symbol* symbol = find_symbol(table, name);
    if (symbol == NULL) {
        // VARIÁVEL NÃO DECLARADA - ERRO!
        fprintf(stderr, "Erro: variável '%s' não declarada\n", name);
        fprintf(stderr, "  Use 'let %s = valor' antes de usar a variável\n", name);
        return 0;  // 0 = erro
    }
    
    if (symbol->type != SYM_NUMBER) {
        fprintf(stderr, "Erro: variável '%s' não é numérica\n", name);
        return 0;
    }
    
    *out_value = symbol->value.num_value;
    return 1;  // 1 = sucesso
}

int symbol_table_exists(SymbolTable* table, const char* name) {
    if (table == NULL || name == NULL) return 0;  // 0 = não existe
    return find_symbol(table, name) != NULL ? 1 : 0;  // 1 = existe, 0 = não existe
}

int symbol_table_count(SymbolTable* table) {
    if (table == NULL) return 0;
    return table->count;
}

void symbol_table_print(SymbolTable* table) {
    if (table == NULL) {
        printf("Tabela de símbolos: NULL\n");
        return;
    }
    
    printf("%s=== TABELA DE SÍMBOLOS (%d variáveis) ===\n%s", 
            COLOR_HEADER, table->count, COLOR_RESET);
    
    Symbol* current = table->head;
    while (current != NULL) {
        printf("  %s: ", current->name);
        
        if (current->type == SYM_NUMBER) {
            printf("[NUM] %.6g", current->value.num_value);
        } else {
            printf("%s[TIPO DESCONHECIDO: %d]%s",
                   COLOR_WARNING, current->type, COLOR_RESET);
        }
        
        printf("\n");
        current = current->next;
    }
    
    if (table->count == 0) {
        printf("  (vazia)\n");
    }
    
    printf("===================================\n");
}

// ============================================
// FUNÇÕES PARA VERSÕES FUTURAS (COMENTADAS)
// ============================================

/*
// Para v0.3.0 (strings) - também com int
int symbol_table_put_string(SymbolTable* table, const char* name, const char* value) {
    if (table == NULL || name == NULL || value == NULL) return 0;
    
    Symbol* symbol = find_symbol(table, name);
    
    if (symbol == NULL) {
        symbol = A89ALLOC(sizeof(Symbol));
        if (symbol == NULL) return 0;
        
        strncpy(symbol->name, name, MAX_VAR_NAME - 1);
        symbol->name[MAX_VAR_NAME - 1] = '\0';
        symbol->type = SYM_STRING;
        strncpy(symbol->value.str_value, value, MAX_STR_VALUE - 1);
        symbol->value.str_value[MAX_STR_VALUE - 1] = '\0';
        
        symbol->next = table->head;
        table->head = symbol;
        table->count++;
    } else {
        if (symbol->type != SYM_STRING) return 0;
        strncpy(symbol->value.str_value, value, MAX_STR_VALUE - 1);
        symbol->value.str_value[MAX_STR_VALUE - 1] = '\0';
    }
    
    return 1;
}
*/

// ============================================
// TESTE RÁPIDO
// ============================================

#ifdef TEST
int main() {
    setup_utf8();
    printf("%s=== Teste Tabela de Símbolos ===\n\n%s",
            COLOR_HEADER, COLOR_RESET);
    
    // 1. Criação
    SymbolTable* table = symbol_table_create();
    if (!table) {
        printf("ERRO: Não foi possível criar tabela\n");
        return 1;
    }
    
    printf("1. Tabela criada (count=%d)\n", symbol_table_count(table));
    
    // 2. Inserção
    printf("\n2. Inserindo variáveis:\n");
    
    if (symbol_table_put(table, "x", 10.5)) {
        printf("   x = 10.5 (inserido)\n");
    } else {
        printf("   ERRO ao inserir x\n");
    }
    
    if (symbol_table_put(table, "y", 20.0)) {
        printf("   y = 20.0 (inserido)\n");
    }
    
    if (symbol_table_put(table, "total", 30.5)) {
        printf("   total = 30.5 (inserido)\n");
    }
    
    // 3. Atualização
    printf("\n3. Atualizando variável existente:\n");
    if (symbol_table_put(table, "x", 15.0)) {
        printf("   x atualizado para 15.0\n");
    }
    
    // 4. Consulta
    printf("\n4. Consultando valores:\n");
    
    double valor;
    if (symbol_table_get(table, "x", &valor)) {
        printf("   x = %.2f\n", valor);
    }
    
    if (symbol_table_get(table, "y", &valor)) {
        printf("   y = %.2f\n", valor);
    }
    
    // 5. Variável não existe
    printf("\n5. Variável não existente:\n");
    if (symbol_table_get(table, "z", &valor)) {
        printf("   z = %.2f (não existe → retorna 0)\n", valor);
    }
    
    // 6. Verificação de existência
    printf("\n6. Verificando existência:\n");
    printf("   'x' existe? %s\n", symbol_table_exists(table, "x") ? "SIM" : "NÃO");
    printf("   'z' existe? %s\n", symbol_table_exists(table, "z") ? "SIM" : "NÃO");
    
    // 7. Impressão completa
    printf("\n7. Tabela completa:\n");
    symbol_table_print(table);
    
    // 8. Limpeza
    printf("\n8. Destruindo tabela...\n");
    symbol_table_destroy(table);
    printf("   Tabela destruída\n");
    
    printf("\n=== Teste concluído ===\n");

    a89check_leaks();
    return 0;
}
#endif