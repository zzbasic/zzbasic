// zztext.c

#include "zztext.h"
#include <string.h>
#include <errno.h>

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
    return txt;
}

void text_free(Text* txt)
{
    if (txt)
    {
        if (txt->data) a89free(txt->data);
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
// Fim de zztext.c
