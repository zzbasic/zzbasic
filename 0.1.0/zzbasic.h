#ifndef ZZBASIC_H
#define ZZBASIC_H

// ============================================
// Configurações Gerais e Includes
// ============================================
#include <stdio.h>
#include <stdbool.h>

// Inclui todos os headers dos módulos
#include "utils.h"
#include "lexer.h"
//#include "parser.h"
//#include "ast.h"
//#include "evaluator.h"

// ============================================
// Constantes Globais
// ============================================
#define ZZ_VERSION "0.1.0"
#define ZZ_PROMPT ">> "

// ============================================
// Protótipos de Funções Globais (main.c)
// ============================================
void run_repl(void);
void run_file(const char* filename);
void print_banner(void);

#endif // ZZBASIC_H