#ifndef ZZBASIC_H
#define ZZBASIC_H

#include <stdio.h>
#include <stdbool.h>

#include "utils.h"

// ================================
// Configurações Básicas
// ================================
#define ZZ_VERSION "0.0.0"
#define ZZ_PROMPT ">> "

// ================================
// Protótipos das Funções Principais
// ================================

// Modos de execução
void run_repl(void);
void run_file(const char* filename);

// Utilitários
void print_banner(void);

#endif // ZZBASIC_H