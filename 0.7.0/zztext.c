// zztext.c

#include <string.h>
#include <errno.h>

#include "color.h"
#include "utils.h"
#include "zztext.h"
#include "object_tracker.h"
#include "debug.h"

// Expande o Text para caber pelo menos 'min_capacity' bytes
static int text_expand(Text* txt, size_t min_capacity)
{
    if (!txt) return 0;
    
    // Se já tem capacidade suficiente, não faz nada
    if (txt->capacity >= min_capacity) return 1;
    
    // Calcula nova capacidade
    // Crescimento 1.5x inicialmente (TEXT_GROWTH_FACTOR)
    // Pode ser alterado no caso de problemas com performance
    size_t new_cap = txt->capacity;
    while (new_cap < min_capacity)
    {
        new_cap = (size_t)(new_cap * TEXT_GROWTH_FACTOR);

        // Proteção contra overflow
        // Se após tentativa de crescimento ficou menor que a capacidade atual
        if (new_cap <= txt->capacity) 
        {
            new_cap = min_capacity;
            break;
        }
    }
    
    // Realoca
    char* new_data = A89REALLOC(txt->data, new_cap);
    if (!new_data) return 0;
    
    txt->data = new_data;
    txt->capacity = new_cap;
    return 1;
}

// =====================================================================
// FUNÇÕES PÚBLICAS
// =====================================================================

Text* text_create(void)
{
    Text* txt = A89ALLOC(sizeof(Text));
    if (!txt) return NULL;
    
    txt->capacity = TEXT_INITIAL_CAPACITY;
    txt->data = A89ALLOC(txt->capacity);
    
    if (!txt->data)
    {
        a89free(txt);
        return NULL;
    }
    
    txt->data[0] = '\0';
    txt->size = 0;

    return txt;
}

Text* text_create_from_string(const char* str)
{
    if (!str) return text_create();
    
    Text* txt = text_create();
    if (!txt) return NULL;
    
    size_t len = strlen(str);
    if (len == 0) return txt;
    
    // Expande para caber a string + \0
    if (!text_expand(txt, len + 1))
    {
        text_free(txt);
        return NULL;
    }
    
    memcpy(txt->data, str, len);
    txt->data[len] = '\0';
    txt->size = len;

    /*
    // Insere objeto na tabela de rastreamento TrackedObject
    int i = object_tracker_add(txt, "Text", str);
    if(i == -1)
    {
        printf("%sErro ao tentar inserir Text em %p na tabela TrackedObject%s\n",
               COLOR_WARNING, txt, COLOR_RESET);
    }

    // Exibe tabela de rastreamento de alocações na heap
    object_tracker_display();
    zzwait();
    */

    return txt;
}

Text* text_create_from_file(const char* filename)
{
    if (!filename || !filename[0]) return NULL;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    Text* txt = text_create();
    if (!txt) {
        fclose(file);
        return NULL;
    }
    
    char buffer[4096];
    size_t bytes;
    
    // =====================================================================
    // fread() - Leitura segura de arquivos
    // =====================================================================
    // size_t fread( void*       destino,     // Onde armazenar os dados
    //               size_t      tam_bytes,   // Tamanho de CADA item (sempre 1 para bytes)
    //               size_t      qtd_itens,   // Quantos itens ler no máximo
    //               FILE*       fonte );     // Arquivo de origem
    //
    // RETORNA: Quantidade de itens EFETIVAMENTE lidos
    //          (em bytes, porque tam_bytes = 1)
    //
    // POR QUE size = 1?
    // - Lê byte a byte → funciona com QUALQUER arquivo
    // - Retorna número exato de bytes lidos
    // - Não perde dados parciais no final do arquivo!
    // =====================================================================
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        // bytes = quantos bytes foram lidos AGORA nesta iteração (pode ser 4096, 500, 1...)

        // Garante espaço para os novos bytes + \0
        if (!text_expand(txt, txt->size + bytes + 1))
        {
            text_free(txt);
            fclose(file);
            return NULL;
        }
        
        memcpy(txt->data + txt->size, buffer, bytes);
        txt->size += bytes;
        txt->data[txt->size] = '\0';
    }
    
    fclose(file);

    /*
    // Insere objeto na tabela de rastreamento TrackedObject
    int i = object_tracker_add(txt, "Text", filename);
    if(i == -1)
    {
        printf("%sErro ao tentar inserir Text em %p na tabela TrackedObject%s\n",
               COLOR_WARNING, txt, COLOR_RESET);
    }

    // Exibe tabela de rastreamento de alocações na heap
    object_tracker_display();
    zzwait();
    */

    return txt;
}

void text_free(Text* txt)
{
    if (!txt)
    { 
        return;
    }
    else
    {
        /*
        int i = find_object_by_pointer(txt);; // ID do objeto na tabela de rastreamento
        if(i != -1)
        {
            object_tracker_remove(i);
            // Exibe tabela de rastreamento de alocações na heap
            object_tracker_display();
            zzwait();
        }
        */
 
        if (txt->data)
        {
            a89free(txt->data);
        }

        a89free(txt);
    }
}

// =====================================================================
// OPERAÇÕES BÁSICAS
// =====================================================================

size_t text_size(const Text* txt)
{
    return txt ? txt->size : 0;
}

const char* text_get(const Text* txt)
{
    return txt ? txt->data : "";
}

int text_save(const Text* txt, const char* filename)
{
    if (!txt || !txt->data || !filename || !filename[0]) return 0;
    if (txt->size == 0) return 0;  // Arquivo vazio
    
    FILE* file = fopen(filename, "wb");
    if (!file) return 0;
    
    size_t written = fwrite(txt->data, 1, txt->size, file);
    fclose(file);
    
    return (written == txt->size);
}

// ============================================================================
// TEXT POOL 
// ============================================================================

// Cria novo pool vazio
TextPool* text_pool_create(void)
{
    TextPool* pool = A89ALLOC(sizeof(TextPool));
    
    pool->texts = A89ALLOC(10 * sizeof(Text*));
    pool->count = 0;
    pool->capacity = 10;
    
    return pool;
}

// Adiciona Text ao pool
void text_pool_add(TextPool* pool, Text* text)
{
    if (!pool || !text) return;
    
    // Se cheio, expande a capacidade
    if (pool->count >= pool->capacity)
    {
        pool->capacity *= 2;
        pool->texts = A89REALLOC(pool->texts, 
                                 pool->capacity * sizeof(Text*));
    }
    
    // Adiciona o Text
    pool->texts[pool->count] = text;
    pool->count++;
}

// Libera todos os Text objects do pool
void text_pool_free(TextPool* pool)
{
    if (!pool) return;
    
    // Libera cada Text object
    for (int i = 0; i < pool->count; i++)
    {
        if (pool->texts[i])
        {
            text_free(pool->texts[i]);
        }
    }
    
    // Libera o array de ponteiros
    a89free(pool->texts);
    
    // Libera o pool
    a89free(pool);
}

// Retorna quantos Text objects estão no pool
int text_pool_count(TextPool* pool)
{
    if (!pool) return 0;
    return pool->count;
}


// Fim de zztext.c
