// zzarray_wrapper.c

#include <string.h>
#include <stdio.h>

#include "evaluator.h"
#include "a89alloc.h"
#include "zzarray.h"
#include "zzarray_wrapper.h"
#include "zztext.h"
#include "result.h"

// ============================================================
// WRAPPERS DE ARRAY
// ============================================================

//===================================================================
// push(arr, 5)
//===================================================================
EvaluatorResult builtin_push(EvaluatorResult* args, int arg_count, int line, int column)
{
    // Valida argumentos
    if (arg_count != 2)
    {
        return create_error_result_fmt(line, column,
            "Array error: push() expects 2 arguments (array, element), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: push() expects array as first argument");
    }
    
    Array* arr = args[0].value.array;
    
    // Converte elemento para void*
    // Por enquanto, vamos armazenar como ponteiro para double
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: memory allocation failed");
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
        return create_error_result_fmt(line, column,
            "Array error: push() with strings not yet supported");
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(line, column,
            "Array error: push() unsupported type");
    }
    
    // Adiciona ao array
    if (!array_push(arr, element))
    {
        a89free(element);
        return create_error_result_fmt(line, column, "Array error: could not push to array");
    }
    
    // Retorna sucesso (sem valor específico)
    return create_success_result_number(1, line, column);
}

//===================================================================
// let x = pop(arr)
//===================================================================
EvaluatorResult builtin_pop(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Array error: pop() expects 1 argument (array), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: pop() expects array as argument");
    }
    
    Array* arr = args[0].value.array;
    
    if (array_is_empty(arr))
    {
        return create_error_result_fmt(line, column, "Array error: cannot pop from empty array");
    }
    
    // Remove do final
    void* element = array_pop(arr);
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: could not pop from array");
    }
    
    // Converte de volta para double
    double value = *(double*)element;
    a89free(element);
    
    return create_success_result_number(value, line, column);
}

//===================================================================
// print len(arr) nl
//===================================================================
EvaluatorResult builtin_len(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Array error: len() expects 1 argument, got %d", arg_count);
    }
    
    // Funciona com ARRAY ou STRING
    if (args[0].type == RESULT_ARRAY)
    {
        Array* arr = args[0].value.array;
        int size = array_size(arr);
        return create_success_result_number((double)size, line, column);
    }
    else if (args[0].type == RESULT_STRING)
    {
        int len = strlen(args[0].value.string);
        return create_success_result_number((double)len, line, column);
    }
    else if (args[0].type == RESULT_TEXT)
    {
        size_t size = text_size(args[0].value.text);
        return create_success_result_number((double)size, line, column);
    }
    else
    {
        return create_error_result_fmt(line, column,
            "Array error: len() expects array, string or text");
    }
}

//===================================================================
// if (is_empty(arr)) then
//     print "Vazio!" nl
// end if
//===================================================================
EvaluatorResult builtin_is_empty(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Array error: is_empty() expects 1 argument, got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: is_empty() expects array as argument");
    }
    
    Array* arr = args[0].value.array;
    int empty = array_is_empty(arr);
    
    return create_success_result_bool(empty, line, column);
}

//===================================================================
// print arr[3] nl    
//===================================================================
EvaluatorResult builtin_get(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(line, column,
            "Array error: get() expects 2 arguments (array, index), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: get() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "Array error: get() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_capacity(arr))
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    void* element = array_get(arr, index);
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: could not get element");
    }
    
    // Converte de volta para double
    double value = *(double*)element;
    
    return create_success_result_number(value, line, column);
}

//===================================================================
// arr[0] = 100   
//===================================================================
EvaluatorResult builtin_set(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 3)
    {
        return create_error_result_fmt(line, column,
            "Array error: set() expects 3 arguments (array, index, value), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: set() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "Array error: set() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_capacity(arr))
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    // Cria novo elemento
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: memory allocation failed");
    }
    
    if (args[2].type == RESULT_NUMBER)
    {
        *element = args[2].value.number;
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(line, column, "Array error: set() unsupported type");
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
        return create_error_result_fmt(line, column, "Array error: could not set element");
    }
    
    return create_success_result_number(1, line, column);
}

//===================================================================
// insert(arr, 1, 50)  
//===================================================================
EvaluatorResult builtin_insert(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 3)
    {
        return create_error_result_fmt(line, column,
            "Array error: insert() expects 3 arguments (array, index, value), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: insert() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "Array error: insert() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Cria novo elemento
    double* element = A89ALLOC(sizeof(double));
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: memory allocation failed");
    }
    
    if (args[2].type == RESULT_NUMBER)
    {
        *element = args[2].value.number;
    }
    else
    {
        a89free(element);
        return create_error_result_fmt(line, column, "Array error: insert() unsupported type");
    }
    
    // Insere no array
    if (!array_insert(arr, index, element))
    {
        a89free(element);
        return create_error_result_fmt(line, column, "Array error: could not insert into array");
    }
    
    return create_success_result_number(1, line, column);
}

//===================================================================
// remove(arr, 1)
//===================================================================
EvaluatorResult builtin_remove(EvaluatorResult* args, int arg_count, int line, int column)
{
    if (arg_count != 2)
    {
        return create_error_result_fmt(line, column,
            "Array error: remove() expects 2 arguments (array, index), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: remove() expects array as first argument");
    }
    
    if (args[1].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "Array error: remove() expects number as index");
    }
    
    Array* arr = args[0].value.array;
    int index = (int)args[1].value.number;
    
    // Valida índice
    if (index < 0 || index >= array_size(arr))
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    // Remove do array
    void* element = array_remove(arr, index);
    if (!element)
    {
        return create_error_result_fmt(line, column, "Array error: could not remove from array");
    }
    
    // Libera elemento removido
    a89free(element);
    
    return create_success_result_number(1, line, column);
}