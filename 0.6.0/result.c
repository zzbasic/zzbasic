// result.c

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "result.h"
#include "color.h"

EvaluatorResult create_success_result_bool(int value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_BOOL;
    result.value.boolean = value;
    result.line = line;      
    result.column = column;  
    return result;    
}

EvaluatorResult create_success_result_number(double value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_NUMBER;
    result.value.number = value;
    result.line = line;      
    result.column = column;  
    return result;
}

EvaluatorResult create_success_result_string(const char* value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_STRING;
    
    if (value)
    {
        int len = strlen(value);
        result.value.string = A89ALLOC(len + 1);
        strncpy(result.value.string, value, len);
        result.value.string[len] = '\0';
    }
    else
    {
        result.value.string = A89ALLOC(1);
        result.value.string[0] = '\0';
    }
    
    result.line = line;
    result.column = column;
    return result;
}

EvaluatorResult create_success_result_array(Array* array, int line, int column)
{
    EvaluatorResult result;
    result.type = RESULT_ARRAY;
    result.value.array = array;
    result.line = line;
    result.column = column;
    return result;
}

EvaluatorResult create_error_result(const char* message, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.type = RESULT_ERROR;
    
    // Calcula tamanho necessário para a mensagem formatada
    int msg_len = snprintf(NULL, 0, "%s[%d:%d] %s%s", 
                           COLOR_ERROR, line, column, message, COLOR_RESET);
    
    result.error_message = A89ALLOC(msg_len + 1);
    if (result.error_message)
    {
        snprintf(result.error_message, msg_len + 1,
                 "%s[%d:%d] %s%s", COLOR_ERROR, line, column, message, COLOR_RESET);
    }
    
    result.line = line;
    result.column = column;
    return result;
}

EvaluatorResult create_error_result_fmt(int line, int column, 
                                       const char* format, ...)
{
    va_list args;
    
    // Primeiro, calcula o tamanho necessário
    va_start(args, format);    
    int msg_len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    
    // Aloca memória
    char* message = A89ALLOC(msg_len + 1);
    if (!message)
    {
        EvaluatorResult result;
        memset(&result, 0, sizeof(EvaluatorResult));
        result.type = RESULT_ERROR;
        result.error_message = A89ALLOC(50);
        if (result.error_message)
            strcpy(result.error_message, "Memory allocation failed");
        return result;
    }
    
    // Formata a mensagem
    va_start(args, format);
    vsnprintf(message, msg_len + 1, format, args);
    va_end(args);
    
    // Cria o resultado
    EvaluatorResult result = create_error_result(message, line, column);
    
    // Libera o buffer temporário
    a89free(message);
    
    return result;
}