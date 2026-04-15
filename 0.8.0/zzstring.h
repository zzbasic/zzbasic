// zzstring.h

#ifndef ZZSTRING_H
#define ZZSTRING_H

#include <stddef.h>

// =====================================================================
// OPERAÇÕES DE STRING
// =====================================================================

// Compara duas strings. Retorna: 1 se iguais, 0 se diferentes
int string_equals(const char* str1, const char* str2);

// Extrai substring
// Parâmetros:
//   str    - string original
//   start  - índice inicial (0-based)
//   length - quantidade de caracteres
// Retorna: nova string alocada na heap
// Exemplo: string_substring("hello", 1, 3) → "ell"
char* string_substring(const char* str, int start, int length);

// Converte para maiúsculas
// Retorna: nova string alocada na heap
char* string_to_upper(const char* str);

// Converte para minúsculas
// Retorna: nova string alocada na heap
char* string_to_lower(const char* str);

// Remove espaços em branco do início e do fim
// Retorna: nova string alocada na heap
// Exemplo: string_trim("  hello  ") → "hello"
char* string_trim(const char* str);

// Divide string por delimitador
// Parâmetros:
//   str       - string original
//   delimiter - delimitador (ex: ",")
//   count     - output: quantidade de partes
// Retorna: array de strings alocado na heap
// Exemplo: string_split("a,b,c", ",", &count) → ["a", "b", "c"], count=3
char** string_split(const char* str, const char* delimiter, int* count);

// Junta array de strings com delimitador
// Parâmetros:
//   strings   - array de strings
//   count     - quantidade de strings
//   delimiter - delimitador (ex: ",")
// Retorna: nova string alocada na heap
// Exemplo: string_join(["a", "b", "c"], 3, ",") → "a,b,c"
char* string_join(char** strings, int count, const char* delimiter);

// Libera array de strings criado por string_split()
void string_free_array(char** strings, int count);

// =====================================================================
// STRING POOL - Gerenciamento Automático de Memória
// =====================================================================

typedef struct
{
    char** strings;     // Array de ponteiros para strings
    int count;          // Quantas strings
    int capacity;       // Capacidade total
} StringPool;

// Funções do String Pool
StringPool* string_pool_create(void);
void string_pool_add(StringPool* pool, char* string);
void string_pool_free(StringPool* pool);
int string_pool_count(StringPool* pool);

#endif
// Fim de zzstring.h
