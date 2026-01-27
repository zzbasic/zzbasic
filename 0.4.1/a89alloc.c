#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>  // UINT_MAX
#include "a89alloc.h"

#define MAX_ALLOCATIONS 1000  // Configurável conforme necessidades

typedef struct {
    void* ptr;          // Ponteiro para a memória alocada
    size_t size;        // Tamanho do bloco alocado em bytes
    char file[256];     // Nome do arquivo onde ocorreu a alocação
    int line;           // Número da linha da alocação
} allocation_info;

// Array principal de controle
static allocation_info allocations[MAX_ALLOCATIONS];

// Contador de alocações ativas
static int total_allocations = 0;


void* a89alloc(size_t size, const char* file, int line) {
    // Validação 1: Verificar limite de alocações
    if (total_allocations >= MAX_ALLOCATIONS) {
        fprintf(stderr,
                "ERRO: Limite máximo de alocações atingido (%d)!\n", 
                MAX_ALLOCATIONS);
        fprintf(stderr,
                "Considere aumentar MAX_ALLOCATIONS ou revisar o código.\n");
        return NULL;
    }
    
    // Validação 2: Verificar tamanho válido
    if (size == 0) {
        fprintf(stderr,
                "AVISO: Tentativa de alocar 0 bytes em %s:%d\n", 
                file, line);
        fprintf(stderr,
                "Esta operação pode indicar um erro lógico.\n");
        return NULL;
    }
    
    // Validação 3: Verificar parâmetros de entrada
    if (file == NULL) {
        fprintf(stderr,
                "ERRO: Parâmetro 'file' é NULL em a89alloc()\n");
        return NULL;
    }
    
    // Alocação usando malloc padrão
    void* ptr = malloc(size);
    
    if (ptr != NULL) {
        // Registro bem-sucedido no sistema de controle
        allocations[total_allocations].ptr = ptr;
        allocations[total_allocations].size = size;
        
        // Cópia segura do nome do arquivo
        strncpy(allocations[total_allocations].file, file, 255);
        // Encerra a string corretamente
        allocations[total_allocations].file[255] = '\0';
        
        allocations[total_allocations].line = line;
        total_allocations++;
        
        // Log informativo (pode ser desabilitado em produção)
        //printf("ALOCACAO: %zu bytes em %s:%d (ptr: %p)\n", 
        //       size, file, line, ptr);
    } else {
        // Tratamento de falha na alocação
        fprintf(stderr,
                "ERRO: Falha na alocação de %zu bytes em %s:%d\n", 
                size, file, line);
        fprintf(stderr,
                "Possíveis causas: memória insuficiente ou fragmentação.\n");
    }
    
    return ptr;
}


void a89free(void* ptr) {
    // Validação: ponteiro NULL é válido (comportamento padrão do free)
    if (ptr == NULL) {
        return;
    }
    
    // Busca linear no array de alocações
    for (int i = 0; i < total_allocations; i++) {
        if (allocations[i].ptr == ptr) {
            // Alocação encontrada - proceder com liberação da memória
            free(ptr);
            
            // Otimização: substituir por último elemento (O(1) vs O(n))
            allocations[i] = allocations[total_allocations - 1];
            total_allocations--;
            
            return;
        }
    }
    
    // Ponteiro não encontrado - possível erro
    fprintf(stderr, "AVISO: Tentativa de liberar ponteiro não rastreado: %p\n", ptr);
    fprintf(stderr, "         Possíveis causas:\n");
    fprintf(stderr, "         - Ponteiro alocado com malloc() padrão\n");
    fprintf(stderr, "         - Double-free (liberação dupla)\n");
    fprintf(stderr, "         - Ponteiro corrompido\n");
    
    // Decisão: liberar mesmo assim para evitar vazamentos
    free(ptr);
}


void a89check_leaks(void) {
    if (total_allocations == 0) {
        printf("SUCESSO: Nenhum vazamento de memoria detectado!\n");
        printf("   Todas as alocacoes foram devidamente liberadas.\n");
        return;
    }
    
    // Cabeçalho do relatório de vazamentos
    printf("VAZAMENTOS DETECTADOS: %d alocacao(oes) nao liberada(s)\n", 
           total_allocations);
    
    // Imprimir linha de separação
    for (int i = 0; i < 60; i++) {
        printf("=");
    }
    printf("\n");
    
    size_t total_leaked = 0;
    size_t max_leak = 0;
    int max_leak_index = 0;
    
    // Análise detalhada de cada vazamento
    for (int i = 0; i < total_allocations; i++) {
        printf("VAZAMENTO #%d:\n", i + 1);
        printf("  Localizacao: %s:%d\n", allocations[i].file, allocations[i].line);
        printf("  Tamanho: %zu bytes\n", allocations[i].size);
        printf("  Endereco: %p\n", allocations[i].ptr);
        
        // Análise de impacto
        if (allocations[i].size > 1024) {
            printf("  IMPACTO ALTO: Vazamento > 1KB\n");
        } else if (allocations[i].size > 100) {
            printf("  IMPACTO MEDIO: Vazamento > 100 bytes\n");
        }
        
        // Imprimir linha de separação
        for (int j = 0; j < 40; j++) {
            printf("-");
        }
        printf("\n");
        
        // Estatísticas
        total_leaked += allocations[i].size;
        if (allocations[i].size > max_leak) {
            max_leak = allocations[i].size;
            max_leak_index = i;
        }
    }
    
    // Resumo estatístico
    printf("\nRESUMO ESTATISTICO:\n");
    printf("  Total vazado: %zu bytes (%.2f KB)\n", 
           total_leaked, (double)total_leaked / 1024.0);
    printf("  Media por vazamento: %.2f bytes\n", 
           (double)total_leaked / total_allocations);
    printf("  Maior vazamento: %zu bytes em %s:%d\n",
           max_leak, allocations[max_leak_index].file, 
           allocations[max_leak_index].line);
    
    // Recomendações
    printf("\nRECOMENDACOES:\n");
    printf("  1. Revisar cada localizacao listada acima\n");
    printf("  2. Adicionar chamadas a89free() correspondentes\n");
    printf("  3. Verificar fluxos de controle (returns, gotos)\n");
    printf("  4. Considerar uso de ferramentas automaticas de analise\n");
}


void a89report_alloc(void) {
    printf("RELATORIO DE ALOCACOES ATIVAS\n");
    
    // Imprimir linha de separação
    for (int i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
    
    printf("Total de alocacoes ativas: %d\n", total_allocations);
    
    if (total_allocations == 0) {
        printf("Nenhuma alocacao ativa no momento.\n");
        printf("   Sistema de memoria limpo.\n");
        
        // Imprimir linha de separação
        for (int i = 0; i < 50; i++) {
            printf("=");
        }
        printf("\n");
        return;
    }
    
    size_t total_memory = 0;
    size_t min_alloc = UINT_MAX;
    size_t max_alloc = 0;
    
    // Processamento de cada alocação
    for (int i = 0; i < total_allocations; i++) {
        printf("\nALOCACAO #%d:\n", i + 1);
        printf("  Tamanho: %zu bytes\n", allocations[i].size);
        printf("  Endereco: %p\n", allocations[i].ptr);
        printf("  Arquivo: %s\n", allocations[i].file);
        printf("  Linha: %d\n", allocations[i].line);
        
        // Estatísticas globais
        total_memory += allocations[i].size;
        if (allocations[i].size < min_alloc) min_alloc = allocations[i].size;
        if (allocations[i].size > max_alloc) max_alloc = allocations[i].size;
    }
    
    // Resumo estatístico
    printf("\nESTATISTICAS GERAIS:\n");
    printf("  Total de memoria alocada: %zu bytes (%.2f KB)\n", 
           total_memory, (double)total_memory / 1024.0);
    printf("  Media por alocacao: %.2f bytes\n", 
           (double)total_memory / total_allocations);
    printf("  Menor alocacao: %zu bytes\n", min_alloc);
    printf("  Maior alocacao: %zu bytes\n", max_alloc);
    
    // Imprimir linha de separação
    for (int i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
}

// fim de a89alloc.c
