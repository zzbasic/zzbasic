// zztext.h

#ifndef ZZTEXT_H
#define ZZTEXT_H

#include <stdio.h>

#include "zzdefs.h"
#include "a89alloc.h"

// TIPO Text - Strings dinâmicas na heap
typedef struct Text {
    char* 	data;       // Conteúdo da string (terminado em '\0')
    size_t 	size;       // Tamanho atual (sem contar '\0')
    size_t 	capacity;   // Capacidade alocada
} Text;

Text* text_create(void);
Text* text_create_from_string(const char* str);
Text* text_create_from_file(const char* filename);

void text_free(Text* txt);

// Retorna o tamanho do texto
size_t text_size(const Text* txt);

// Retorna o conteúdo como string C (só leitura!)
const char* text_get(const Text* txt);

// Salva o conteúdo em arquivo
int text_save(const Text* txt, const char* filename);

// =====================================================================
// OPERAÇÕES FUTURAS (adicione quando precisar)
// =====================================================================
// int text_append(Text* dest, const char* src);
// void text_clear(Text* txt);
// Text* text_clone(const Text* src);

#endif
// Fim de zztext.h
