// zzbasic.h

#ifndef ZZBASIC_H
#define ZZBASIC_H

#include <stdio.h>

#include "symbol_table.h"
#include "zzdefs.h"
#include "ast.h"

typedef struct {
    char lines[MAX_PROGRAM_LINES][PROGRAM_LINE_SIZE];
    int line_count;
    int in_program_mode;
    ASTNode* ast;
} ReplProgram;

const char* get_os_name(void);
int is_empty_line(const char *line);
char* read_file(const char* filename, size_t* length);
void run_repl(void);
void run_file(const char* filename);
int has_zz_extension(const char* filename);
void list_variables(SymbolTable* symbols);
void show_tokens(const char* code);
void show_ast(const char* code);

// Helper functions for REPL multi-line
ASTNode* compile_program(ReplProgram* program);
void parse_range(const char* arg, int max_lines, int* start, int* end);

void print_banner(void);

#endif // ZZBASIC_H
// Fim de zzbasic.h