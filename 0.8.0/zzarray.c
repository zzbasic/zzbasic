// zzarray.c

#include <string.h>
#include <math.h>

#include "zzarray.h"
#include "result.h"


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

    // Inicializar novas posições com NULL
    memset(new_elements + array->capacity, 0, (new_cap - array->capacity) * sizeof(void*));
    
    array->elements = new_elements;
    array->capacity = new_cap;
    return 1;
}


Array* array_create(void)
{
    return array_create_with_capacity(ARRAY_INITIAL_CAPACITY);
}

Array* array_create_with_capacity(int capacity)
{
    Array* array = A89ALLOC(sizeof(Array));
    
    int cap = (capacity > 0) ? capacity : ARRAY_INITIAL_CAPACITY;
    
    array->capacity = cap;
    array->elements = A89ALLOC(array->capacity * sizeof(void*));

    // Inicializar com NULL
    memset(array->elements, 0, array->capacity * sizeof(void*));
    
    array->size = 0;
    array->element_type = -1; 

    return array;
}

void array_free(Array* array)
{
    if (array)
    {
        if (array->elements)
        {
            // Libera cada elemento
            for (int i = 0; i < array->size; i++)
            {
                if (array->elements[i])
                {
                    // Só libera NUMBER (alocados por set/push)
                    // STRING e TEXT são gerenciado pelo pool
                    if (array->element_type == RESULT_NUMBER)
                    {
                        a89free(array->elements[i]);
                    }
                }
            }
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
    if (!array || index < 0) return 0;
    
    // Se índice está além da capacidade, expande
    if (index >= array->capacity)
    {
        if (!array_expand(array, index + 1))
            return 0;
    }
    
    // Se índice está além do tamanho, expande o tamanho
    if (index >= array->size)
    {
        // Preenche posições vazias com NULL
        for (int i = array->size; i <= index; i++)
        {
            array->elements[i] = NULL;
        }
        array->size = index + 1;
    }
    
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

// Compara dois arrays
// Retorna: 1 se iguais, 0 se diferentes
// 
// Semântica:
// - NULL  == NULL   1 (ambos não existem, são iguais)
// - NULL  == array  0 (um existe, outro não)
// - array == NULL   0 (um existe, outro não)
// - array == array  compara conteúdo
//
int array_equals(Array* arr1, Array* arr2)
{
    // Ambos NULL: iguais
    if (!arr1 && !arr2) return 1;
    
    // Um é NULL, outro não: diferentes
    if (!arr1 || !arr2) return 0;
    
    // Ambos não-NULL: compara tamanho
    if (array_size(arr1) != array_size(arr2))
    {
        return 0;
    }
    
    // Compara cada elemento
    for (int i = 0; i < array_size(arr1); i++)
    {
        void* elem1 = array_get(arr1, i);
        void* elem2 = array_get(arr2, i);
        
        // Se ambos são NULL (empty)
        if (elem1 == NULL && elem2 == NULL)
        {
            continue;
        }
        
        // Se um é NULL e outro não
        if ((elem1 == NULL) != (elem2 == NULL))
        {
            return 0;
        }
        
        // Compara baseado no tipo
        if (arr1->element_type == RESULT_NUMBER)
        {
            double val1 = *(double*)elem1;
            double val2 = *(double*)elem2;
            
            if (fabs(val1 - val2) >= EPSILON)
            {
                return 0;
            }
        }
        else if (arr1->element_type == RESULT_STRING)
        {
            char* str1 = (char*)elem1;
            char* str2 = (char*)elem2;
            
            if (strcmp(str1, str2) != 0)
            {
                return 0;
            }
        }
        else if (arr1->element_type == RESULT_TEXT)
        {
            Text* txt1 = (Text*)elem1;
            Text* txt2 = (Text*)elem2;
            
            // Compara conteúdo de Text
            const char* content1 = text_get(txt1);
            const char* content2 = text_get(txt2);
            
            if (strcmp(content1, content2) != 0)
            {
                return 0;
            }
        }
    }
    
    return 1;  // Arrays são iguais
}

// Troca dois elementos de posição
// Retorna: 1 se sucesso, 0 se erro
int array_swap(Array* array, int i, int j)
{
    if (!array) return 0;
    
    // Valida índices
    if (i < 0 || i >= array_size(array) || j < 0 || j >= array_size(array))
    {
        return 0;
    }
    
    // Se índices são iguais, nada a fazer
    if (i == j) return 1;
    
    // Troca os elementos
    void* temp = array->elements[i];
    array->elements[i] = array->elements[j];
    array->elements[j] = temp;
    
    return 1;
}

// ============================================================================
// INSERTION SORT - Versão Genérica com Flag
// ============================================================================
// Ordena array em ordem crescente ou decrescente
// Retorna: 1 se sucesso, 0 se erro
// Complexidade: O(n²) pior caso, O(n) melhor caso
// Espaço: O(1)
//
// Parâmetros:
//   array: Array a ser ordenado
//   ascending: 1 para crescente, 0 para decrescente
//
static int array_sort_internal(Array* array, int ascending)
{
    if (!array || array_size(array) == 0) return 1;  // Array vazio é válido

    // Só pode ordenar arrays de NUMBER ou STRING
    if (array->element_type != RESULT_NUMBER && array->element_type != RESULT_STRING)
    {
        return 0;  // Erro: tipo não suportado para ordenação
    }
    
    int n = array_size(array);
    
    // Insertion Sort
    for (int i = 1; i < n; i++)
    {
        void* key_elem = array_get(array, i);
        
        // Se elemento é NULL (empty), pula
        if (key_elem == NULL)
            continue;
        

        int compare_result = 0;
        
        if (array->element_type == RESULT_NUMBER)
        {
            double key = *(double*)key_elem;
            int j = i - 1;
            
            while (j >= 0)
            {
                void* elem_j = array_get(array, j);
                if (elem_j == NULL)
                {
                    j--;
                    continue;
                }
                
                double val_j = *(double*)elem_j;
                
                if (ascending)
                {
                    if (val_j <= key) break;
                }
                else
                {
                    if (val_j >= key) break;
                }
                
                if (!array_swap(array, j, j + 1))
                {
                    return 0;
                }
                
                j--;
            }
        }
        else if (array->element_type == RESULT_STRING)
        {
            char* key = (char*)key_elem;
            int j = i - 1;
            
            while (j >= 0)
            {
                void* elem_j = array_get(array, j);
                if (elem_j == NULL)
                {
                    j--;
                    continue;
                }
                
                char* str_j = (char*)elem_j;
                int cmp = strcmp(str_j, key);
                
                if (ascending)
                {
                    if (cmp <= 0) break;  // str_j <= key
                }
                else
                {
                    if (cmp >= 0) break;  // str_j >= key
                }
                
                if (!array_swap(array, j, j + 1))
                {
                    return 0;
                }
                
                j--;
            }
        }
    }
    
    return 1;
}

// ============================================================================
// WRAPPERS PÚBLICOS
// ============================================================================

// Ordena array em ordem crescente
int array_sort(Array* array)
{
    return array_sort_internal(array, 1);  // ascending = 1
}

// Ordena array em ordem decrescente
int array_rsort(Array* array)
{
    return array_sort_internal(array, 0);  // ascending = 0
}

// Fim de zzarray.c