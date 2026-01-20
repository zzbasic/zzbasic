#ifndef ZZBASIC_H
#define ZZBASIC_H

#include <stdio.h>

#define ZZ_VERSION "0.1.0"
#define ZZ_PROMPT ">> "

const char* get_os_name(void);
int is_empty_line(const char *line);
char* read_file(const char* filename, size_t* length);
void run_repl(void);
void run_file(const char* filename);
void print_banner(void);

#endif // ZZBASIC_H