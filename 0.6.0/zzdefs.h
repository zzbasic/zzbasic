// zzdefs.h

#ifndef ZZ_DEFS_H
#define ZZ_DEFS_H

#define ZZ_VERSION "0.6.0"
#define ZZ_PROMPT ">> "

// TAMANHO MAXIMO
#define NUMBER_SIZE 		24
#define VARNAME_SIZE 		32

// REPL Multi-line 
#define MAX_PROGRAM_LINES	50

// Tamanhos de buffers locais/temporários
#define ERROR_MSG_SIZE      256    // Para mensagens de erro locais
#define TEMP_BUFFER_SIZE    512    // Para buffers temporários
#define FORMAT_BUFFER_SIZE  1024   // Para formatação de saída

#endif
// Fim de zzdefs.h