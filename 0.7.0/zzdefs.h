// zzdefs.h

#ifndef ZZ_DEFS_H
#define ZZ_DEFS_H

#include <stddef.h> // size_t

#define ZZ_VERSION "0.7.0"
#define ZZ_PROMPT ">> "

// TAMANHO MAXIMO
#define BUFFER_SIZE       	256    // Para mensagens de erro LONGAS
#define NUMBER_SIZE 		24
#define VARNAME_SIZE 		32
#define TOKENTEXT_SIZE   	64    // Para texto de token (números, operadores)
#define STRING_SIZE 		128
#define PROGRAM_LINE_SIZE	128

// REPL Multi-line 
#define MAX_PROGRAM_LINES	50

// TIPO TEXT
#define TEXT_INITIAL_CAPACITY 	64 	// Tamanho inicial (64 bytes)
#define TEXT_GROWTH_FACTOR 		1.5 // Fator de crescimento (1.5x)

// ============================================
// Modo de Execução
// ============================================
#define MODE_FILE  0  // Interpretando arquivo .zz
#define MODE_REPL  1  // REPL interativo
// Flag que indica se o interpretador está em MODE_FILE ou MODE_REPL
extern int execution_mode;

// Flag que indica se houver erro no Lexer
extern int has_lexical_error;

// Flag que indica se houver erro no Evaluator
extern int has_evaluation_error;

// ============================================
// Contexto de Execução para Evaluator 
//
// para permitir que mensagens de erro do Evaluator
// possam exibir a linha com erro
// ============================================
typedef struct {
    const char* source;      // Código-fonte completo
    size_t source_size;      // Tamanho do source
} EvalSourceContext;

extern EvalSourceContext eval_src_ctx;

#endif
// Fim de zzdefs.h