// zztext_wrapper.c

#include <string.h>
#include <stdio.h>

#include "evaluator.h"
#include "a89alloc.h"
#include "zztext.h"
#include "zztext_wrapper.h"
#include "result.h"
#include "scope.h"

// ============================================================
// WRAPPERS DE TEXT
// ============================================================

//===================================================================
// text(string) - Cria novo Text object a partir de string
//===================================================================
EvaluatorResult builtin_text(
    EvaluatorResult* args, 
    int arg_count, 
    int line, 
    int column,
    ScopeStack* scope_stack)
{
    // Valida argumentos
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Text error: text() expects 1 argument (string), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "Text error: text() argument must be string, got %s",
            result_type_to_string(args[0].type));
    }
    
    // Cria Text object a partir da string
    Text* text_obj = text_create_from_string(args[0].value.string);
    
    if (!text_obj)
    {
        return create_error_result_fmt(line, column,
            "Text error: failed to create text object");
    }
    
    // Adiciona ao pool do escopo atual
    if (scope_stack)
    {
        scope_stack_add_text(scope_stack, text_obj);
    }
    
    return create_success_result_text(text_obj, line, column);
}

//===================================================================
// load(filename) - Carrega arquivo em Text object
//===================================================================
EvaluatorResult builtin_load(
    EvaluatorResult* args, 
    int arg_count, 
    int line, 
    int column,
    ScopeStack* scope_stack)
{
    // Valida argumentos
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Text error: load() expects 1 argument (filename), got %d", arg_count);
    }
    
    if (args[0].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "Text error: load() argument must be string (filename), got %s",
            result_type_to_string(args[0].type));
    }
    
    const char* filename = args[0].value.string;
    
    // Valida se filename não está vazio
    if (!filename || filename[0] == '\0')
    {
        return create_error_result_fmt(line, column,
            "Text error: load() filename cannot be empty");
    }
    
    // Carrega arquivo em Text object
    Text* text_obj = text_create_from_file(filename);
    
    if (!text_obj)
    {
        return create_error_result_fmt(line, column,
            "Text error: failed to load file '%s'", filename);
    }
    
    // Adiciona ao pool do escopo atual
    if (scope_stack)
    {
        scope_stack_add_text(scope_stack, text_obj);
    }
    
    return create_success_result_text(text_obj, line, column);
}

//===================================================================
// save(text, filename) - Salva Text object em arquivo
//===================================================================
EvaluatorResult builtin_save(
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
            "Text error: save() expects 2 arguments (text, filename), got %d", arg_count);
    }
    
    // Primeiro argumento deve ser TEXT
    if (args[0].type != RESULT_TEXT)
    {
        return create_error_result_fmt(line, column,
            "Text error: save() first argument must be text, got %s",
            result_type_to_string(args[0].type));
    }
    
    // Segundo argumento deve ser STRING (filename)
    if (args[1].type != RESULT_STRING)
    {
        return create_error_result_fmt(line, column,
            "Text error: save() second argument must be string (filename), got %s",
            result_type_to_string(args[1].type));
    }
    
    Text* text_obj = args[0].value.text;
    const char* filename = args[1].value.string;
    
    // Valida se filename não está vazio
    if (!filename || filename[0] == '\0')
    {
        return create_error_result_fmt(line, column,
            "Text error: save() filename cannot be empty");
    }
    
    // Salva Text object em arquivo
    if (!text_save(text_obj, filename))
    {
        return create_error_result_fmt(line, column,
            "Text error: failed to save to file '%s'", filename);
    }
    
    // Retorna sucesso (sem valor específico)
    return create_success_result_number(1, line, column);
}

// Fim de zztext_wrapper.c
