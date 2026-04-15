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
EvaluatorResult builtin_push(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
    int value_type = args[1].type;
    
    // Se array está vazio, define o tipo
    if (arr->element_type == -1)
    {
        arr->element_type = value_type;
    }
    // Se array já tem tipo, valida
    else if (arr->element_type != value_type)
    {
        return create_error_result_fmt(line, column,
            "Array error: type mismatch - array is of type %s, but got type %s",
            result_type_to_string(arr->element_type),
            result_type_to_string(value_type));
    }
    
    void* element = NULL;
    
    if (args[1].type == RESULT_NUMBER)
    {
        double* num_element = A89ALLOC(sizeof(double));
        *num_element = args[1].value.number;
        element = num_element;
    }
    else if (args[1].type == RESULT_STRING)
    {
        // Copia a string
        char* str_copy = A89ALLOC(strlen(args[1].value.string) + 1);
        strcpy(str_copy, args[1].value.string);
        
        // Adiciona ao StringPool para gerenciar liberação
        if (scope_stack)
        {
            scope_stack_add_string(scope_stack, str_copy);
        }
        
        element = str_copy;
    }
    else
    {
        return create_error_result_fmt(line, column, 
            "Array error: push() unsupported type (expected number or string)");
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
EvaluatorResult builtin_pop(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
    if (arr->element_type == RESULT_NUMBER)
    {
        double value = *(double*)element;
        a89free(element);  // Libera apenas NUMBER
        return create_success_result_number(value, line, column);
    }
    else if (arr->element_type == RESULT_STRING)
    {
        char* string = (char*)element;
        return create_success_result_string(string, line, column);  // NÃO libera aqui, StringPool libera
    }
    else
    {
        return create_error_result_fmt(line, column, "Array error: unsupported array type");
    }
}

//===================================================================
// print len(arr) nl
//===================================================================
EvaluatorResult builtin_len_array(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Array error: len(array) expects 1 argument, got %d", arg_count);
    }

    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Array error: len(array) expects array as argument");
    }

    Array* arr = args[0].value.array;
    int size = array_size(arr);
    return create_success_result_number((double)size, line, column); 
}

//===================================================================
// if (is_empty(arr)) then
//     print "Vazio!" nl
// end if
//===================================================================
EvaluatorResult builtin_is_empty(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
EvaluatorResult builtin_get(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
    if (index < 0 || index >= array_size(arr))
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    void* element = array_get(arr, index);
    if (!element)
    {
        //return create_error_result_fmt(line, column, "Array error: could not get element");
        return create_success_result_empty(line, column);
    }
    
    // Retorna o tipo correto
    if (arr->element_type == RESULT_NUMBER)
    {
        double value = *(double*)element;
        return create_success_result_number(value, line, column);
    }
    else if (arr->element_type == RESULT_STRING)
    {
        char* string = (char*)element;
        return create_success_result_string(string, line, column);
    }
    else
    {
        return create_error_result_fmt(line, column,
            "Array error: unsupported array type");
    }
}

//===================================================================
// arr[0] = 100   
//===================================================================
EvaluatorResult builtin_set(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
    if (index < 0)
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    // Valida tipo 
    int value_type = args[2].type;
    
    // Se array está vazio, define o tipo
    if (arr->element_type == -1)
    {
        arr->element_type = value_type;
    }
    // Se array já tem tipo, valida
    else if (arr->element_type != value_type)
    {
        return create_error_result_fmt(line, column,
            "Array error: type mismatch - array is of type %s, but got type %s",
            result_type_to_string(arr->element_type),
            result_type_to_string(value_type)
            );
    }

    // Cria novo elemento
    void* element = NULL;

    if (args[2].type == RESULT_NUMBER)
    {
        double* num_element = A89ALLOC(sizeof(double));
        if (!num_element)
        {
            return create_error_result_fmt(line, column, "Array error: memory allocation failed");
        }
        *num_element = args[2].value.number;
        element = num_element;
    }
    else if (args[2].type == RESULT_STRING)
    {
        // Copia a string
        char* str_copy = A89ALLOC(strlen(args[2].value.string) + 1);
        strcpy(str_copy, args[2].value.string);
        
        // Adiciona ao StringPool para gerenciar liberação
        if (scope_stack)
        {
            scope_stack_add_string(scope_stack, str_copy);
        }
        
        element = str_copy;
    }
    else
    {
        return create_error_result_fmt(line, column, 
            "Array error: set() unsupported type (expected number or string)");
    }
    
    /****************************************************************
    LIBERA ELEMENTO ANTIGO SE FOR NUMBER

    * Pega o elemento antigo na posição [index]
    * Se é NUMBER: libera (foi alocado por set())
    * Se é TEXT/STRING: NÃO libera (gerenciado pelo pool)

    RESULT_NUMBER: alocado como double* em set() → deve liberar
    RESULT_TEXT: gerenciado pelo TextPool → não liberar aqui
    RESULT_STRING: gerenciado pelo StringPool → não liberar aqui
    ****************************************************************/
    void* old_element = array_get(arr, index);
    if (old_element && args[2].type == RESULT_NUMBER)
    {
        a89free(old_element);
    }
    
    // Insere novo elemento no array.
    if (!array_set(arr, index, element))
    {
        if (args[2].type == RESULT_NUMBER)
        {
            a89free(element);
        }
        return create_error_result_fmt(line, column, "Array error: could not set element");
    }

    return create_success_result_number(1, line, column);
}

//===================================================================
// insert(arr, 1, 50)  
//===================================================================
EvaluatorResult builtin_insert(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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

    if (index < 0)
    {
        return create_error_result_fmt(line, column,
            "Array error: array index out of bounds: %d", index);
    }
    
    // Validar tipo
    int value_type = args[2].type;
    
    if (arr->element_type == -1)
    {
        arr->element_type = value_type;
    }
    else if (arr->element_type != value_type)
    {
        return create_error_result_fmt(line, column,
            "Array error: type mismatch - array is of type %s, but got type %s",
            result_type_to_string(arr->element_type),
            result_type_to_string(value_type));
    }
    
    void* element = NULL;
    
    if (args[2].type == RESULT_NUMBER)
    {
        double* num_element = A89ALLOC(sizeof(double));
        *num_element = args[2].value.number;
        element = num_element;
    }
    else if (args[2].type == RESULT_STRING)
    {
        char* str_copy = A89ALLOC(strlen(args[2].value.string) + 1);
        strcpy(str_copy, args[2].value.string);
        
        if (scope_stack)
        {
            scope_stack_add_string(scope_stack, str_copy);
        }
        
        element = str_copy;
    }
    else
    {
        return create_error_result_fmt(line, column, 
            "Array error: insert() unsupported type (expected number or string)");
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
EvaluatorResult builtin_remove(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
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
    
    // Libera elemento removido (apenas se for NUMBER); String é liberado pelo pool
    if (arr->element_type == RESULT_NUMBER)
    {
        a89free(element);
    }
    
    return create_success_result_number(1, line, column);
}

EvaluatorResult builtin_swap(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    // Valida número de argumentos
    if (arg_count != 3)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: swap() expects 3 arguments (array, index1, index2), got %d",
            arg_count);
    }
    
    // Valida tipo do primeiro argumento (deve ser array)
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: swap() first argument must be array");
    }
    
    // Valida tipos dos índices (devem ser números)
    if (args[1].type != RESULT_NUMBER || args[2].type != RESULT_NUMBER)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: swap() indices must be numbers");
    }
    
    Array* arr = args[0].value.array;
    int i = (int)args[1].value.number;
    int j = (int)args[2].value.number;
    
    // Chama a função de troca
    if (!array_swap(arr, i, j))
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: swap() index out of bounds");
    }
    
    // Retorna o array modificado
    return create_success_result_array(arr, line, column);
}

EvaluatorResult builtin_sort(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    // Valida número de argumentos
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: sort() expects 1 argument (array), got %d",
            arg_count);
    }
    
    // Valida tipo do argumento (deve ser array)
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: sort() argument must be array");
    }
    
    Array* arr = args[0].value.array;

    // Só pode ordenar arrays de NUMBER ou STRING
    if (arr->element_type != RESULT_NUMBER && arr->element_type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "Array error: sort() only works with numeric or string arrays");
    }
    
    // Chama a função de ordenação
    if (!array_sort(arr))
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: sort() failed");
    }
    
    // Retorna o array modificado
    return create_success_result_array(arr, line, column);
}

EvaluatorResult builtin_rsort(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    // Valida número de argumentos
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: rsort() expects 1 argument (array), got %d",
            arg_count);
    }
    
    // Valida tipo do argumento (deve ser array)
    if (args[0].type != RESULT_ARRAY)
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: rsort() argument must be array");
    }
    
    Array* arr = args[0].value.array;

    // Só pode ordenar arrays de NUMBER ou STRING
    if (arr->element_type != RESULT_NUMBER && arr->element_type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "Array error: rsort() only works with numeric or string arrays");
    }
    
    // Chama a função de ordenação reversa
    if (!array_rsort(arr))
    {
        return create_error_result_fmt(line, column,
            "Evaluator error: rsort() failed");
    }
    
    // Retorna o array modificado
    return create_success_result_array(arr, line, column);
}
