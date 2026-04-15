// zzstring.c

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "zzstring.h"
#include "a89alloc.h"

// Se ambas são NULL → retorna 1 (iguais)
// Se uma é NULL e outra não → retorna 0 (diferentes)
// Usa strcmp() para comparar
int string_equals(const char* str1, const char* str2)
{
    if (!str1 && !str2) return 1;      // Ambas NULL
    if (!str1 || !str2) return 0;      // Uma é NULL
    return strcmp(str1, str2) == 0;    // Compara conteúdo
}


// string_substring("hello", 1, 3) → "ell"
// string_substring("hello", 0, 2) → "he"
// string_substring("hello", 2, 10) → "llo" (ajusta length)
char* string_substring(const char* str, int start, int length)
{
    if (!str) return NULL;
    
    int str_len = strlen(str);
    
    // Validações e ajustes
    if (start < 0) start = 0;
    if (length < 0) length = 0;

    if (start >= str_len)
    {
        // Retorna string vazia
        char* result = A89ALLOC(1);
        result[0] = '\0';
        return result;
    }
    
    // Ajusta length se necessário
    if (start + length > str_len)
    {
        length = str_len - start;
    }
    
    // Aloca memória para resultado
    char* result = A89ALLOC(length + 1);
    
    // Copia substring
    strncpy(result, str + start, length);
    result[length] = '\0';
    
    return result;
}

char* string_to_upper(const char* str)
{
    if (!str) return NULL;
    
    int len = strlen(str);
    char* result = A89ALLOC(len + 1);
    
    for (int i = 0; i < len; i++)
    {
        result[i] = toupper((unsigned char)str[i]);
    }
    result[len] = '\0';
    
    return result;
}

char* string_to_lower(const char* str)
{
    if (!str) return NULL;
    
    int len = strlen(str);
    char* result = A89ALLOC(len + 1);
    
    for (int i = 0; i < len; i++)
    {
        result[i] = tolower((unsigned char)str[i]);
    }
    result[len] = '\0';
    
    return result;
}

// string_trim("  hello  ") → "hello"
// string_trim("   ") → ""
// string_trim("hello") → "hello"
char* string_trim(const char* str)
{
    if (!str) return NULL;
    
    int len = strlen(str);
    int start = 0;
    int end = len - 1;
    
    // Encontra primeiro caractere não-espaço
    while (start <= end && isspace((unsigned char)str[start]))
    {
        start++;
    }
    
    // Encontra último caractere não-espaço
    while (end >= start && isspace((unsigned char)str[end]))
    {
        end--;
    }
    
    // Calcula tamanho da substring
    int result_len = (start <= end) ? (end - start + 1) : 0;
    
    // Aloca e copia
    char* result = A89ALLOC(result_len + 1);
    if (result_len > 0)
    {
        strncpy(result, str + start, result_len);
    }
    result[result_len] = '\0';
    
    return result;
}


// str: string a dividir
// delimiter: delimitador (ex: ",")
// count: output - quantidade de partes
//
// string_split("a,b,c", ",", &count) → ["a", "b", "c"], count=3
char** string_split(const char* str, const char* delimiter, int* count)
{
    if (!str || !delimiter || !count) return NULL;
    
    // Cria cópia da string para não modificar original
    char* str_copy = A89ALLOC(strlen(str) + 1);
    strcpy(str_copy, str);
    
    // Primeira passagem: conta quantas partes
    *count = 0;
    char* temp = str_copy;
    while (*temp)
    {
        if (strstr(temp, delimiter) == temp)
        {
            temp += strlen(delimiter);
        } else
        {
            (*count)++;
            char* next = strstr(temp, delimiter);
            if (next)
            {
                temp = next + strlen(delimiter);
            }
            else
            {
                break;
            }
        }
    }
    
    // Se string vazia, retorna array com 1 elemento vazio
    if (*count == 0)
    {
        char** result = A89ALLOC(sizeof(char*) * 1);
        result[0] = A89ALLOC(1);
        result[0][0] = '\0';
        *count = 1;
        a89free(str_copy);
        return result;
    }
    
    // Aloca array de ponteiros
    char** result = A89ALLOC(sizeof(char*) * (*count));
    
    // Segunda passagem: extrai partes
    strcpy(str_copy, str);  // Restaura cópia
    int idx = 0;
    char* token = strtok(str_copy, delimiter);
    
    while (token && idx < *count)
    {
        result[idx] = A89ALLOC(strlen(token) + 1);
        strcpy(result[idx], token);
        token = strtok(NULL, delimiter);
        idx++;
    }
    
    a89free(str_copy);
    return result;
}

// strings: array de strings
// count: quantidade de strings
// delimiter: delimitador

// string_join(["a", "b", "c"], 3, ",") → "a,b,c"
char* string_join(char** strings, int count, const char* delimiter)
{
    if (!strings || count <= 0 || !delimiter) return NULL;
    
    // Calcula tamanho total necessário
    int total_size = 0;
    for (int i = 0; i < count; i++)
    {
        if (strings[i])
        {
            total_size += strlen(strings[i]);
        }
    }
    // Adiciona espaço para delimitadores
    total_size += (count - 1) * strlen(delimiter);
    
    // Aloca resultado
    char* result = A89ALLOC(total_size + 1);
    result[0] = '\0';
    
    // Junta strings
    for (int i = 0; i < count; i++)
    {
        if (strings[i])
        {
            strcat(result, strings[i]);
        }
        
        // Adiciona delimitador (exceto após última string)
        if (i < count - 1) {
            strcat(result, delimiter);
        }
    }
    
    return result;
}

void string_free_array(char** strings, int count)
{
    if (!strings) return;
    
    for (int i = 0; i < count; i++)
    {
        if (strings[i])
        {
            a89free(strings[i]);
        }
    }
    
    a89free(strings);
}

// =====================================================================
// STRING POOL 
// =====================================================================

#define STRINGPOOL_INITIAL_CAPACITY 10

StringPool* string_pool_create(void)
{
    StringPool* pool = A89ALLOC(sizeof(StringPool));
    
    pool->strings = A89ALLOC(sizeof(char*) * STRINGPOOL_INITIAL_CAPACITY);
    pool->count = 0;
    pool->capacity = STRINGPOOL_INITIAL_CAPACITY;
    
    return pool;
}

void string_pool_add(StringPool* pool, char* string)
{
    if (!pool || !string) return;
    
    // Se atingiu capacidade, expande
    if (pool->count >= pool->capacity)
    {
        pool->capacity *= 2;
        char** new_strings = A89ALLOC(sizeof(char*) * pool->capacity);
        
        for (int i = 0; i < pool->count; i++)
        {
            new_strings[i] = pool->strings[i];
        }
        
        a89free(pool->strings);
        pool->strings = new_strings;
    }
    
    // Adiciona string
    pool->strings[pool->count] = string;
    pool->count++;
}

void string_pool_free(StringPool* pool)
{
    if (!pool) return;
    
    // Libera cada string
    for (int i = 0; i < pool->count; i++)
    {
        if (pool->strings[i])
        {
            a89free(pool->strings[i]);
        }
    }
    
    // Libera array de ponteiros
    a89free(pool->strings);
    
    // Libera a pool
    a89free(pool);
}

int string_pool_count(StringPool* pool)
{
    return pool ? pool->count : 0;
}
// Fim de zzstring.c