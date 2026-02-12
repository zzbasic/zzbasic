// zzdefs.h

#ifndef ZZ_DEFS_H
#define ZZ_DEFS_H

#define ZZ_VERSION "0.6.0"
#define ZZ_PROMPT ">> "

// TAMANHO MAXIMO
#define BUFFER_SIZE       	256    // Para mensagens de erro LONGAS
#define NUMBER_SIZE 		24
#define VARNAME_SIZE 		32
#define TOKENTEXT_SIZE   	64    // Para texto de token (números, operadores)
#define STRING_SIZE 		128
#define PROGRAM_LINE_SIZE	80

// REPL Multi-line 
#define MAX_PROGRAM_LINES	50

// TIPO TEXT
#define TEXT_INITIAL_CAPACITY 	64 	// Tamanho inicial (64 bytes)
#define TEXT_GROWTH_FACTOR 		1.5 // Fator de crescimento (1.5x)

#endif
// Fim de zzdefs.h