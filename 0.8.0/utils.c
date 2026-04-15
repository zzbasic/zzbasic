// utils.c

#include <stdio.h>
#include <string.h>

#include "color.h"
#include "utils.h"
#include "zzarray_wrapper.h"
#include "zztext_wrapper.h"
#include "zzstring_wrapper.h"

//#include "debug.h"

// UTF-8 setup for Windows
#ifdef _WIN32
#include <windows.h>
void setup_utf8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
#else
void setup_utf8() {
    // On Linux/macOS, UTF-8 is already the default
}
#endif

void zzwait() {
    printf("\n""Press ENTER to continue...");
    getchar(); // Wait for user to press Enter
    printf("\n");
}


/********************************************************************
Exibe uma linha de código-fonte com uma seta apontando para o erro

Parâmetros:
    * source      Ponteiro para o código-fonte completo
    * source_size Tamanho total do código-fonte em bytes
    * line        Número da linha (1-indexado)
    * column      Número da coluna (1-indexado)
 
Exemplo de saída:
    Linha 5: let x = 10 + 
                      ↑
*********************************************************************/
void display_error_location(const char* source, int source_size, int line, int column)
{
    // Validação de entrada
    if (!source || source_size <= 0 || line <= 0 || column <= 0)
    {
        return;
    }

    // ========================================
    // ETAPA 1: Encontrar o início da linha
    // ========================================
    int line_start = 0;
    int current_line = 1;
    
    for (int i = 0; i < source_size && current_line < line; i++)
    {
        if (source[i] == '\n')
        {
            current_line++;
            line_start = i + 1;
        }
    }
    
    // Se a linha solicitada não existe, retorna
    if (current_line < line)
    {
        return;
    }

    // ========================================
    // ETAPA 2: Encontrar o final da linha
    // ========================================
    int line_end = line_start;
    while (line_end < source_size && 
           source[line_end] != '\n' && 
           source[line_end] != '\0')
    {
        line_end++;
    }
    
    int display_length = line_end - line_start;

    // ========================================
    // ETAPA 3: Exibir a linha com número
    // ========================================
    fprintf(stderr, "  Linha %d: ", line);
    fwrite(&source[line_start], 1, display_length, stderr);
    fprintf(stderr, "\n");

    // ========================================
    // ETAPA 4: Calcular indentação da seta
    // ========================================
    // Formato: "  Linha XXX: " onde XXX é o número da linha
    // Precisamos calcular quantos espaços adicionar antes da seta
    
    // Largura do número da linha (ex: "5" = 1 char, "123" = 3 chars)
    int line_num_width = snprintf(NULL, 0, "%d", line);
    
    // Indentação total:
    // "  Linha " = 8 caracteres
    // + largura do número da linha
    // + ": " = 2 caracteres
    int indent = 8 + line_num_width + 2;
    
    // Imprime espaços iniciais para alinhar com o início da linha
    for (int i = 0; i < indent; i++)
    {
        fprintf(stderr, " ");
    }
    
    // ========================================
    // ETAPA 5: Imprimir espaços até a coluna
    // ========================================
    // A coluna é 1-indexada, então subtraímos 1
    for (int i = 1; i < column; i++)
    {
        fprintf(stderr, " ");
    }
    
    // ========================================
    // ETAPA 6: Imprimir a seta apontando
    // ========================================
    fprintf(stderr, "%s↑%s\n", COLOR_ERROR, COLOR_RESET);
}

//===================================================================
// len() - Genérico para Array, Text e String
// Identifica o tipo e chama a função específica
//===================================================================
EvaluatorResult builtin_len(
    EvaluatorResult* args,
    int arg_count,
    int line,
    int column,
    ScopeStack* scope_stack)
{
    if (arg_count != 1)
    {
        return create_error_result_fmt(line, column,
            "Error: len() expects 1 argument, got %d", arg_count);
    }
    
    // Identifica o tipo e chama a função específica
    switch (args[0].type)
    {
        case RESULT_ARRAY:
            return builtin_len_array(args, arg_count, line, column, scope_stack);
        
        case RESULT_TEXT:
            return builtin_len_text(args, arg_count, line, column, scope_stack);
        
        case RESULT_STRING:
            return builtin_len_string(args, arg_count, line, column, scope_stack);
        
        default:
            return create_error_result_fmt(line, column,
                "Error: len() expects array, text or string, got %s",
                (args[0].type == RESULT_NUMBER ? "number" :
                 args[0].type == RESULT_BOOL ? "boolean" : "unknown"));
    }
}

// Fim de utils.c