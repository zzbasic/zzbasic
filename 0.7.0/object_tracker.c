// object_tracker.c
// Implementação do módulo de rastreamento de alocações na heap

#include <stdio.h>
#include <string.h>

#include "object_tracker.h"
#include "color.h"

// ===================================================================
// VARIÁVEIS GLOBAIS
// ===================================================================

// Array fixo de objetos rastreados
static TrackedObject tracked_objects[MAX_TRACKED_OBJECTS];

// Próximo ID a ser atribuído
static int next_id = 1;

// ===================================================================
// FUNÇÕES PRIVADAS
// ===================================================================

/**
 * Procura um slot vazio no array de rastreamento
 * return - Índice do slot vazio ou -1 se array cheio
 */
static int find_empty_slot(void)
{
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].id == 0)
        {
            return i;
        }
    }
    return -1;
}

/**
 * Procura um objeto pelo ID
 * return - Índice do objeto ou -1 se não encontrado
 */
static int find_object_by_id(int id)
{
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

/**
 * Procura um objeto pelo ponteiro
 * return - ID do objeto ou -1 se não encontrado
 */
int find_object_by_pointer(void* ptr)
{
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].pointer == ptr)
        {
            return tracked_objects[i].id;
        }
    }
    return -1;
}

// ===================================================================
// FUNÇÕES PÚBLICAS
// ===================================================================

void object_tracker_init(void)
{
    // Zera o array
    memset(tracked_objects, 0, sizeof(tracked_objects));
    
    // Reseta ID
    next_id = 1;
}

int object_tracker_add(void* pointer, const char* type, const char* info)
{
    if (!pointer || !type)
    {
        return -1;
    }
    
    // Procura slot vazio
    int slot = find_empty_slot();
    if (slot == -1)
    {
        fprintf(stderr, "%sObject Tracker Error: array cheio (MAX=%d)%s\n",
                COLOR_ERROR, MAX_TRACKED_OBJECTS, COLOR_RESET);
        return -1;
    }
    
    // Preenche o slot
    tracked_objects[slot].id = next_id;
    tracked_objects[slot].pointer = pointer;
    tracked_objects[slot].type = type;

    // Copia a string info
    if (info)
    {
        strncpy(tracked_objects[slot].info, info, STRING_SIZE - 1);
        tracked_objects[slot].info[STRING_SIZE - 1] = '\0';
    }
    else
    {
        tracked_objects[slot].info[0] = '\0';
    }

    tracked_objects[slot].is_freed = 0;
    
    int current_id = next_id;
    next_id++;
    
    return current_id;
}

void object_tracker_remove(int id)
{
    if (id <= 0)
    {
        return;
    }
    
    int slot = find_object_by_id(id);
    if (slot != -1)
    {
        tracked_objects[slot].is_freed = 1;
    }
}

void object_tracker_display(void)
{
    printf("\n");
    printf("%s╔════════════════════════════════════════════════════════════════════╗%s\n",
           COLOR_INFO, COLOR_RESET);
    printf("%s║                   OBJECT ALLOCATION TABLE                          ║%s\n",
           COLOR_INFO, COLOR_RESET);
    printf("%s╚════════════════════════════════════════════════════════════════════╝%s\n",
           COLOR_INFO, COLOR_RESET);
    
    printf("\n%-4s | %-8s | %-14s   | %-20s | %-8s\n",
           "ID", "Type", "Address", "Info", "Status");
    printf("-----|----------|------------------|----------------------|----------\n");
    
    int active = 0;
    int freed = 0;
    
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].id != 0)
        {
            const char* status = tracked_objects[i].is_freed ? "FREED" : "ACTIVE";
            const char* status_color = tracked_objects[i].is_freed ? COLOR_WARNING : COLOR_SUCCESS;
            
            // Limita info a 20 caracteres
            //char info_truncated[21];
            //strncpy(info_truncated, tracked_objects[i].info, 20);
            //info_truncated[20] = '\0';
            
            printf("%-4d | %-8s | %p | %-20s | %s%s%s\n",
                   tracked_objects[i].id,
                   tracked_objects[i].type,
                   tracked_objects[i].pointer,
                   tracked_objects[i].info,
                   status_color, status, COLOR_RESET);
            
            if (tracked_objects[i].is_freed)
            {
                freed++;
            }
            else
            {
                active++;
            }
        }
    }
    
    printf("\n%sTotal: %d active, %d freed%s\n\n",
           COLOR_INFO, active, freed, COLOR_RESET);
}

int object_tracker_count_active(void)
{
    int count = 0;
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].id != 0 && !tracked_objects[i].is_freed)
        {
            count++;
        }
    }
    return count;
}

int object_tracker_count_freed(void)
{
    int count = 0;
    for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
    {
        if (tracked_objects[i].id != 0 && tracked_objects[i].is_freed)
        {
            count++;
        }
    }
    return count;
}

// Fim de object_tracker.c