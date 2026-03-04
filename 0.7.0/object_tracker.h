// object_tracker.h
// Módulo genérico para rastreamento de alocações de objetos na heap
// Permite visualizar quais objetos foram alocados, liberados ou estão ativos

#ifndef OBJECT_TRACKER_H
#define OBJECT_TRACKER_H

#define MAX_TRACKED_OBJECTS 1000

#include "zzdefs.h"

// Estrutura que representa um objeto rastreado
typedef struct
{
    int id;                     // ID único do objeto (0 = slot vazio)
    void* pointer;              // Endereço do objeto na memória
    const char* type;           // Tipo do objeto ("Text", "Array", "String", etc)
    char info[STRING_SIZE];     // Informação adicional (conteúdo, tamanho, etc)
    int is_freed;               // 1 = liberado, 0 = ativo
} TrackedObject;

// ===================================================================
// FUNÇÕES PÚBLICAS
// ===================================================================

/**
 * Inicializa o rastreador
 * Deve ser chamado uma única vez no início do programa
 */
void object_tracker_init(void);

/**
 * Adiciona um objeto ao rastreador
 * 
 * pointer - Endereço do objeto alocado
 * type    - Tipo do objeto (ex: "Text", "Array")
 * info    - Informação adicional (ex: conteúdo, tamanho)
 * 
 * return  - ID único do objeto (> 0) ou -1 se erro (array cheio)
 */
int object_tracker_add(void* pointer, const char* type, const char* info);

/**
 * Remove um objeto do rastreador (marca como liberado)
 * 
 * id - ID do objeto retornado por object_tracker_add()
 */
void object_tracker_remove(int id);

/**
 * Procura um objeto pelo ponteiro
 * return - ID do objeto ou -1 se não encontrado
 */
int find_object_by_pointer(void* ptr);

/**
 * Exibe tabela de alocações
 * Mostra todos os objetos rastreados, seu status (ativo/liberado) e informações
 */
void object_tracker_display(void);

/**
 * Retorna quantidade de objetos ativos
 * 
 * return - Número de objetos ainda alocados
 */
int object_tracker_count_active(void);

/**
 * Retorna quantidade de objetos liberados
 * 
 * Número de objetos marcados como liberados
 */
int object_tracker_count_freed(void);

#endif // OBJECT_TRACKER_H