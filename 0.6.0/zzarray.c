// zzarray.c

#include "zzarray.h"
#include <string.h>


static int array_expand(Array* array, int min_capacity)
{
    if (!array) return 0;
    
    // Se já tem capacidade suficiente, não faz nada
    if (array->capacity >= min_capacity) return 1;
    
    // Calcula nova capacidade 
    int new_cap = array->capacity;
    while (new_cap < min_capacity)
    {
        new_cap = (int)(new_cap * ARRAY_GROWTH_FACTOR);
        
        // Proteção contra overflow
        if (new_cap <= array->capacity)
        {
            new_cap = min_capacity;
            break;
        }
    }
    
    // Realoca array de ponteiros
    void** new_elements = A89REALLOC(array->elements, new_cap * sizeof(void*));
    if (!new_elements) return 0;
    
    array->elements = new_elements;
    array->capacity = new_cap;
    return 1;
}


Array* array_create(void)
{
    Array* array = A89ALLOC(sizeof(Array));
    if (!array) return NULL;
    
    array->capacity = ARRAY_INITIAL_CAPACITY;
    array->elements = A89ALLOC(array->capacity * sizeof(void*));
    
    if (!array->elements)
    {
        a89free(array);
        return NULL;
    }
    
    array->size = 0;
    return array;
}

void array_free(Array* array)
{
    if (array)
    {
        if (array->elements)
        {
            a89free(array->elements);
        }
        a89free(array);
    }
}

// Adiciona elemento no final (push)
int array_push(Array* array, void* element)
{
    if (!array) return 0;
    
    // Expande se necessário
    if (array->size >= array->capacity)
    {
        if (!array_expand(array, array->capacity + 1))
            return 0;
    }
    
    array->elements[array->size] = element;
    array->size++;
    return 1;
}

// Insere elemento em posição específica
int array_insert(Array* array, int index, void* element)
{
    if (!array || index < 0 || index > array->size) return 0;
    
    // Expande se necessário
    if (array->size >= array->capacity)
    {
        if (!array_expand(array, array->capacity + 1))
            return 0;
    }
    
    // Desloca elementos para a direita
    for (int i = array->size; i > index; i--)
    {
        array->elements[i] = array->elements[i - 1];
    }
    
    array->elements[index] = element;
    array->size++;
    return 1;
}

// Remove e retorna último elemento (pop)
void* array_pop(Array* array)
{
    if (!array || array->size == 0) return NULL;
    
    array->size--;
    return array->elements[array->size];
}

// Remove elemento em posição específica
void* array_remove(Array* array, int index)
{
    if (!array || index < 0 || index >= array->size) return NULL;
    
    void* element = array->elements[index];
    
    // Desloca elementos para a esquerda
    for (int i = index; i < array->size - 1; i++)
    {
        array->elements[i] = array->elements[i + 1];
    }
    
    array->size--;
    return element;
}

// Obtém elemento em posição específica
void* array_get(Array* array, int index)
{
    if (!array || index < 0 || index >= array->size) return NULL;
    return array->elements[index];
}

// Define elemento em posição específica
int array_set(Array* array, int index, void* element)
{
    if (!array || index < 0 || index >= array->size) return 0;
    array->elements[index] = element;
    return 1;
}

// Retorna quantidade de elementos
int array_size(const Array* array)
{
    return array ? array->size : 0;
}

// Retorna capacidade alocada
int array_capacity(const Array* array)
{
    return array ? array->capacity : 0;
}

// Verifica se array está vazio
int array_is_empty(const Array* array)
{
    return array ? (array->size == 0) : 1;
}

// Fim de zzarray.c