// zzarray_wrapper.c

#include <string.h>
#include <stdio.h>

#include "zzarray.h"
#include "zzarray_wrapper.h"
#include "zztext.h"
#include "a89alloc.h"

// ============================================================
// WRAPPERS DE ARRAY
// ============================================================

//===================================================================
// push(arr, 5)
//===================================================================
static EvaluatorResult builtin_push(EvaluatorResult* args, int arg_count)
{
    // Valida argumentos
    if (arg_count != 2)
    {
        return create_error_result_fmt(0, 0,
            "push() expects 2 arguments (array, element), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "push() expects array as first argument");
    }
    
    Array* arr = args[0].value.array;
    
    // Converte elemento para void*
    // Por enquanto, vamos armazenar como ponteiro para double
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Memory allocation failed");
    }
    
    // Copia valor do argumento
    if (args[1].type == RESULT_NUMBER)
    {
        *element = args[1].value.number;
    }
    else if (args[1].type == RESULT_STRING)
    {
        // Para strings, precisaríamos armazenar char*
        // Por enquanto, retorna erro
        a89free(element);
        return create_error_result_fmt(0, 0,
            "push() with strings not yet supported");
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(0, 0,
            "push() unsupported type");
    }
    
    // Adiciona ao array
    if (!array_push(arr, element))
    {
        a89free(element);
        return create_error_result_fmt(0, 0, "Could not push to array");
    }
    
    // Retorna sucesso (sem valor específico)
    return create_success_result_number(1, 0, 0);
}

//===================================================================
// let x = pop(arr)
//===================================================================
static EvaluatorResult builtin_pop(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(0, 0,
            "pop() expects 1 argument (array), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "pop() expects array as argument");
    }
    
    Array* arr = args[0].value.array;
    
    if (array_is_empty(arr))
    {
        return create_error_result_fmt(0, 0, "Cannot pop from empty array");
    }
    
    // Remove do final
    void* element = array_pop(arr);
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Could not pop from array");
    }
    
    // Converte de volta para double
    double value = *(double*)element;
    a89free(element);
    
    return create_success_result_number(value, 0, 0);
}

//===================================================================
// print len(arr) nl
//===================================================================
static EvaluatorResult builtin_len(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(0, 0,
            "len() expects 1 argument, got %d", arg_count);
    }
    
    // Funciona com ARRAY ou STRING
    if (args[0].type == RESULT_ARRAY)
    {
        Array* arr = args[0].value.array;
        int size = array_size(arr);
        return create_success_result_number((double)size, 0, 0);
    }
    else if (args[0].type == RESULT_STRING)
    {
        int len = strlen(args[0].value.string);
        return create_success_result_number((double)len, 0, 0);
    }
    else if (args[0].type == RESULT_TEXT)
    {
        size_t size = text_size(args[0].value.text);
        return create_success_result_number((double)size, 0, 0);
    }
    else
    {
        return create_error_result_fmt(0, 0,
            "len() expects array, string or text");
    }
}

//===================================================================
// if (is_empty(arr)) then
//     print "Vazio!" nl
// end if
//===================================================================
static EvaluatorResult builtin_is_empty(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(0, 0,
            "is_empty() expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "is_empty() expects array as argument");
    }
    
    Array* arr = args[0].value.array;
    int empty = array_is_empty(arr);
    
    return create_success_result_bool(empty, 0, 0);
}

//===================================================================
// print arr[3] nl    
//===================================================================
static EvaluatorResult builtin_get(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(0, 0,
            "get() expects 2 arguments (array, index), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "get() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(0, 0,
            "get() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_size(arr))
    {
        return create_error_result_fmt(0, 0,
            "Array index out of bounds: %d", index);
    }
    
    void* element = array_get(arr, index);
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Could not get element");
    }
    
    // Converte de volta para double
    double value = *(double*)element;
    
    return create_success_result_number(value, 0, 0);
}

//===================================================================
// arr[0] = 100   
//===================================================================
static EvaluatorResult builtin_set(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 3)
    {
        return create_error_result_fmt(0, 0,
            "set() expects 3 arguments (array, index, value), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "set() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(0, 0,
            "set() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_size(arr))
    {
        return create_error_result_fmt(0, 0,
            "Array index out of bounds: %d", index);
    }
    
    // Cria novo elemento
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Memory allocation failed");
    }
    
    if (args[2].type == RESULT_NUMBER)
    {
        *element = args[2].value.number;
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(0, 0, "set() unsupported type");
    }
    
    // Libera elemento antigo
    void* old_element = array_get(arr, index);
    if (old_element)
    {
        a89free(old_element);
    }
    
    // Define novo elemento
    if (!array_set(arr, index, element))
    {
        a89free(element);
        return create_error_result_fmt(0, 0, "Could not set element");
    }
    
    return create_success_result_number(1, 0, 0);
}

//===================================================================
// insert(arr, 1, 50)  
//===================================================================
static EvaluatorResult builtin_insert(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 3)
    {
        return create_error_result_fmt(0, 0,
            "insert() expects 3 arguments (array, index, value), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "insert() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(0, 0,
            "insert() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Cria novo elemento
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Memory allocation failed");
    }
    
    if (args[2].type == RESULT_NUMBER)
    {
        *element = args[2].value.number;
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(0, 0, "insert() unsupported type");
    }
    
    // Insere no array
    if (!array_insert(arr, index, element))
    {
        a89free(element);
        return create_error_result_fmt(0, 0, "Could not insert into array");
    }
    
    return create_success_result_number(1, 0, 0);
}

//===================================================================
// remove(arr, 1)
//===================================================================
static EvaluatorResult builtin_remove(EvaluatorResult* args, int arg_count)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(0, 0,
            "remove() expects 2 arguments (array, index), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(0, 0,
            "remove() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(0, 0,
            "remove() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_size(arr))
    {
        return create_error_result_fmt(0, 0,
            "Array index out of bounds: %d", index);
    }
    
    // Remove do array
    void* element = array_remove(arr, index);
    if (!element)
    {
        return create_error_result_fmt(0, 0, "Could not remove from array");
    }
    
    // Libera elemento removido
    a89free(element);
    
    return create_success_result_number(1, 0, 0);
}