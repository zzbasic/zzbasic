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
    strncpy(result.value.string, value, sizeof(result.value.string) - 1);
    result.value.string[sizeof(result.value.string) - 1] = '\0';
    result.line = line;
    result.column = column;
    return result;
}

EvaluatorResult create_success_result_text(Text* text, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult));
    result.type = RESULT_TEXT;
    result.value.text = text;
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
    snprintf(result.error_message, sizeof(result.error_message), 
             "%s[%d:%d] %s%s", COLOR_ERROR, line, column, message, COLOR_RESET);
    result.line = line;
    result.column = column;
    return result;
}

// Versão no estilo printf para mensagens formatadas
EvaluatorResult create_error_result_fmt(int line, int column, 
                                       const char* format, ...)
{
    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    return create_error_result(message, line, column);
}