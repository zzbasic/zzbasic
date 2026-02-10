// zzdefs.h

#ifndef ZZ_DEFS_H
#define ZZ_DEFS_H

#define ZZ_VERSION "0.5.4"
#define ZZ_PROMPT ">> "

// TAMANHO MAXIMO
#define BUFFER_SIZE       	256    // Para mensagens de erro LONGAS
#define NUMBER_SIZE 		64
#define VARNAME_SIZE 		64
#define TOKENTEXT_SIZE   	128    // Para texto de token (números, operadores)
#define STRING_SIZE 		256


// REPL Multi-line Program Storage
#define MAX_PROGRAM_LINES 100
#define PROGRAM_LINE_SIZE BUFFER_SIZE

#endif
// Fim de zzdefs.h