// result.h

#ifndef RESULT_H
#define RESULT_H

#include "zzdefs.h"
#include "zztext.h"
#include "zzarray.h"

typedef enum
{
    RESULT_ERROR,
    RESULT_BOOL,
    RESULT_NUMBER,
    RESULT_STRING,
    RESULT_ARRAY
} ResultType;

typedef struct
{
    ResultType type;
    union {
        int     boolean;
        double  number;
        char*   string;
        Array*  array;
    } value;
    char* error_message;
    int line;   
    int column; 
} EvaluatorResult;


// Cria resultado de sucesso com valor booleano
EvaluatorResult create_success_result_bool(int value, int line, int column);

// Cria resultado de sucesso com valor numérico
EvaluatorResult create_success_result_number(double value, int line, int column);

// Cria resultado de sucesso com string
EvaluatorResult create_success_result_string(const char* value, int line, int column);

// Cria resultado de sucesso com Text
EvaluatorResult create_success_result_text(Text* text, int line, int column);

// Cria resultado de sucesso com Array
EvaluatorResult create_success_result_array(Array* arr, int line, int column);

// Cria resultado de erro com mensagem simples
EvaluatorResult create_error_result(const char* message, int line, int column);

// Cria resultado de erro com mensagem formatada (printf-style)
EvaluatorResult create_error_result_fmt(int line, int column, 
                                       const char* format, ...);

#endif // RESULT_H