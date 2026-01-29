#ifndef A89ALLOC_H
#define A89ALLOC_H

#include <stddef.h>

/********************************************************************
Função principal de alocação com rastreamento
Parâmetros:
    size - tamanho em bytes a ser alocado
    file - nome do arquivo fonte (capturado automaticamente)
    line - número da linha (capturado automaticamente)

Retorno:
    Ponteiro para a memória alocada ou NULL em caso de falha
********************************************************************/
void* a89alloc(size_t size, const char* file, int line);


/********************************************************************
Macro para facilitar o uso da função a89alloc
Captura automaticamente __FILE__ e __LINE__
********************************************************************/
#define A89ALLOC(size) a89alloc(size, __FILE__, __LINE__)


/********************************************************************
Função para liberação de memória com rastreamento
Parâmetro:
	ptr - ponteiro a ser liberado
********************************************************************/
void a89free(void* ptr);

/********************************************************************
Função para verificar vazamentos de memória
Deve ser chamada antes do encerramento do programa
********************************************************************/
void a89check_leaks(void);

/********************************************************************
Função para gerar relatório de alocações ativas
Útil para depuração 
********************************************************************/
void a89report_alloc(void);

#endif // A89ALLOC_H
