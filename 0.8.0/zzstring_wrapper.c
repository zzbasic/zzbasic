// zzstring_wrapper.c

#include <string.h>
#include <stdio.h>

#include "evaluator.h"
#include "a89alloc.h"
#include "zzstring.h"
#include "zzstring_wrapper.h"
#include "zzarray.h"
#include "result.h"

// ============================================================
// WRAPPERS DE STRING
// ============================================================

//===================================================================
// len(string) - Retorna tamanho da string
// Exemplo: print len("hello") nl  # 5
//===================================================================
EvaluatorResult builtin_len_string(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "String error: len(string) expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: len(string) expects string as argument");
    }
    
    int len = strlen(args[0].value.string);
    return create_success_result_number((double)len, line, column);
}

//===================================================================
// substr(string, start, length) - Extrai substring
// Exemplo: print substr("hello", 1, 3) nl  # "ell"
//===================================================================
EvaluatorResult builtin_substr(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 3)
    {
        return create_error_result_fmt(line, column,
            "String error: substr() expects 3 arguments (string, start, length), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: substr() expects string as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "String error: substr() expects number as start index");
    }
    
    if (args[2].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "String error: substr() expects number as length");
    }
    
    const char* str = args[0].value.string;
    int start = (int)args[1].value.number;
    int length = (int)args[2].value.number;
    
    // Chama função core
    char* result = string_substring(str, start, length);
    
    if (!result)
    {
        return create_error_result_fmt(line, column, "String error: substr() failed");
    }
    
    // Adiciona ao StringPool do escopo
    Scope* current_scope = scope_get_current(scope_stack);
    if (current_scope && current_scope->string_pool)
    {
        string_pool_add(current_scope->string_pool, result);
    }
    
    return create_success_result_string(result, line, column);
}

//===================================================================
// upper(string) - Converte para maiúsculas
// Exemplo: print upper("hello") nl  # "HELLO"
//===================================================================
EvaluatorResult builtin_upper(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "String error: upper() expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: upper() expects string as argument");
    }
    
    const char* str = args[0].value.string;
    
    // Chama função core
    char* result = string_to_upper(str);
    
    if (!result)
    {
        return create_error_result_fmt(line, column, "String error: upper() failed");
    }
    
    // Adiciona ao StringPool do escopo
    Scope* current_scope = scope_get_current(scope_stack);
    if (current_scope && current_scope->string_pool)
    {
        string_pool_add(current_scope->string_pool, result);
    }
    
    return create_success_result_string(result, line, column);
}

//===================================================================
// lower(string) - Converte para minúsculas
// Exemplo: print lower("HELLO") nl  # "hello"
//===================================================================
EvaluatorResult builtin_lower(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "String error: lower() expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: lower() expects string as argument");
    }
    
    const char* str = args[0].value.string;
    
    // Chama função core
    char* result = string_to_lower(str);
    
    if (!result)
    {
        return create_error_result_fmt(line, column, "String error: lower() failed");
    }
    
    // Adiciona ao StringPool do escopo
    Scope* current_scope = scope_get_current(scope_stack);
    if (current_scope && current_scope->string_pool)
    {
        string_pool_add(current_scope->string_pool, result);
    }
    
    return create_success_result_string(result, line, column);
}

//===================================================================
// trim(string) - Remove espaços em branco
// Exemplo: print trim("  hello  ") nl  # "hello"
//===================================================================
EvaluatorResult builtin_trim(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "String error: trim() expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: trim() expects string as argument");
    }
    
    const char* str = args[0].value.string;
    
    // Chama função core
    char* result = string_trim(str);
    
    if (!result)
    {
        return create_error_result_fmt(line, column, "String error: trim() failed");
    }
    
    // Adiciona ao StringPool do escopo
    Scope* current_scope = scope_get_current(scope_stack);
    if (current_scope && current_scope->string_pool)
    {
        string_pool_add(current_scope->string_pool, result);
    }
    
    return create_success_result_string(result, line, column);
}

//===================================================================
// split(string, delimiter) - Divide em array
// Exemplo: let parts = split("a,b,c", ",")
//===================================================================
EvaluatorResult builtin_split(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(line, column,
            "String error: split() expects 2 arguments (string, delimiter), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: split() expects string as first argument");
    }
    
    if (args[1].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: split() expects string as delimiter");
    }
    
    const char* str = args[0].value.string;
    const char* delimiter = args[1].value.string;
    int count = 0;
    
    // Chama função core
    char** parts = string_split(str, delimiter, &count);
    
    if (!parts)
    {
        return create_error_result_fmt(line, column, "String error: split() failed");
    }
    
    // Cria array para armazenar as partes
    Array* result_array = array_create();
    if (!result_array)
    {
        string_free_array(parts, count);
        return create_error_result_fmt(line, column, "String error: could not create array");
    }
    
    // Adiciona cada parte ao array
    for (int i = 0; i < count; i++)
    {
        // Adiciona cada string ao StringPool
        Scope* current_scope = scope_get_current(scope_stack);
        if (current_scope && current_scope->string_pool)
        {
            string_pool_add(current_scope->string_pool, parts[i]);
        }
        
        // Cria um wrapper para armazenar no array
        // (Arrays armazenam void*, precisamos de um wrapper)
        char** string_ptr = A89ALLOC(sizeof(char*));
        if (!string_ptr)
        {
            array_free(result_array);
            return create_error_result_fmt(line, column, "String error: memory allocation failed");
        }
        
        *string_ptr = parts[i];
        array_push(result_array, string_ptr);
    }
    
    // Libera array de ponteiros (não as strings, pois estão no pool)
    a89free(parts);
    
    return create_success_result_array(result_array, line, column);
}

//===================================================================
// join(array, delimiter) - Junta array em string
// Exemplo: let result = join(parts, "-")
//===================================================================
EvaluatorResult builtin_join(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(line, column,
            "String error: join() expects 2 arguments (array, delimiter), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "String error: join() expects array as first argument");
    }
    
    if (args[1].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "String error: join() expects string as delimiter");
    }
    
    Array* arr = args[0].value.array;
    const char* delimiter = args[1].value.string;
    int count = array_size(arr);
    
    // Extrai strings do array
    char** strings = A89ALLOC(sizeof(char*) * count);
    if (!strings)
    {
        return create_error_result_fmt(line, column, "String error: memory allocation failed");
    }
    
    for (int i = 0; i < count; i++)
    {
        void* element = array_get(arr, i);
        if (!element)
        {
            a89free(strings);
            return create_error_result_fmt(line, column, "String error: invalid array element");
        }
        
        // Desempacota o ponteiro para string
        strings[i] = *(char**)element;
    }
    
    // Chama função core
    char* result = string_join(strings, count, delimiter);
    
    a89free(strings);
    
    if (!result)
    {
        return create_error_result_fmt(line, column, "String error: join() failed");
    }
    
    // Adiciona ao StringPool do escopo
    Scope* current_scope = scope_get_current(scope_stack);
    if (current_scope && current_scope->string_pool)
    {
        string_pool_add(current_scope->string_pool, result);
    }
    
    return create_success_result_string(result, line, column);
}

// Fim de zzstring_wrapper.c