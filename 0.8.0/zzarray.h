// zzarray.h

#ifndef ZZARRAY_H
#define ZZARRAY_H

#include <stdio.h>
#include "zzdefs.h"
#include "a89alloc.h"

#define ARRAY_INITIAL_CAPACITY  16
#define ARRAY_GROWTH_FACTOR     1.5

// =====================================================================
// TIPO Array - Array dinâmico na heap
// =====================================================================
// Armazena ponteiros genéricos (void*)
// Cada elemento pode ser: número, string, text, outro array, etc
// =====================================================================
typedef struct Array
{
    void**  elements;       // Array de ponteiros (alocado na heap)
    int     size;          // Quantidade de elementos atuais
    int     capacity;       // Capacidade alocada
} Array;


Array* array_create(void);

Array* array_create_with_capacity(int capacity);

void array_free(Array* array);

// Adiciona elemento no final (push)
int array_push(Array* array, void* element);

// Insere elemento em posição específica
int array_insert(Array* array, int index, void* element);

// Remove e retorna último elemento (pop)
void* array_pop(Array* array);

// Remove elemento em posição específica
void* array_remove(Array* array, int index);

// Define elemento em posição específica
void* array_get(Array* array, int index);

// Insere elemento em posição específica
int array_set(Array* array, int index, void* element);

// Retorna quantidade de elementos
int array_size(const Array* array);

// Retorna capacidade alocada
int array_capacity(const Array* array);

// Verifica se array está vazio
int array_is_empty(const Array* array);

// Compara dois arrays
// Retorna: 1 se iguais, 0 se diferentes
// 
// Semântica:
// - NULL  == NULL   1 (ambos não existem, são iguais)
// - NULL  == array  0 (um existe, outro não)
// - array == NULL   0 (um existe, outro não)
// - array == array  compara conteúdo
int array_equals(Array* arr1, Array* arr2);

// Troca dois elementos de posição
// Retorna: 1 se sucesso, 0 se erro
int array_swap(Array* array, int i, int j);

// Ordena array em ordem crescente
int array_sort(Array* array);

// Ordena array em ordem decrescente
int array_rsort(Array* array);

#endif
// Fim de zzarray.h