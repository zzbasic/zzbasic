#ifndef ZZBASIC_H
#define ZZBASIC_H

#include <stdio.h>

#include "symbol_table.h"

#define ZZ_VERSION "0.3.0"
#define ZZ_PROMPT ">> "

const char* get_os_name(void);
int is_empty_line(const char *line);
char* read_file(const char* filename, size_t* length);
void run_repl(void);
void run_file(const char* filename);
int has_zz_extension(const char* filename);
void list_variables(SymbolTable* symbols);
void show_tokens(const char* code);
void show_ast(const char* code);

void print_banner(void);

#endif // ZZBASIC_H