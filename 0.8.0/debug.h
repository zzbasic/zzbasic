/********************************************************************
debug.h - Macros para debug 

COMO USAR:

int minha_funcao(int x)
{
    DEBUG_CHECKPOINT();  // Mostra onde está
    
    int y = x * 2;
    DEBUG_VAR(y);  // Mostra valor de y
    
    char* nome = "teste";
    DEBUG_VAR_STR(nome);  // Mostra string
    
    void* ptr;
    DEBUG_VAR_PTR(ptr);  // Mostra ponteiro

    double pi = 3.14159;
    DEBUG_VAR_DBL(pi);  // Mostra double
    
    DEBUG_MSG("Chegou aqui!");  // Mensagem personalizada
    
    if (x > 10) {
        DEBUG_IF(x > 10);  // Só mostra se x > 10
    }
    
    return y;
}

*********************************************************************

SAIDA ESPERADA:

*********************************************************************
DEBUG_CHECKPOINT(); // Mostra onde está

    DEBUG CHECK POINT
      Arquivo : parser.c
      Função  : minha_funcao
      Linha   : 42
*********************************************************************
int y = x * 2; // Mostra valor de y
DEBUG_VAR(y); 

    DEBUG CHECK POINT
      Arquivo  : parser.c
      Função   : minha_funcao
      Linha    : 45
      Variável : y = 10
*********************************************************************
char* nome = "teste"; // Mostra string
DEBUG_VAR_STR(nome);

    DEBUG CHECK POINT
      Arquivo  : parser.c
      Função   : minha_funcao
      Linha    : 48
      Variável : nome = "teste"
*********************************************************************
double pi = 3.14159; // Mostra double
DEBUG_VAR_DBL(pi);

DEBUG CHECK POINT
  Arquivo  : parser.c
  Função   : minha_funcao
  Linha    : 51
  Variável : pi = 3.14159
*********************************************************************
DEBUG_MSG("Chegou aqui!");  // Mensagem personalizada

    DEBUG CHECK POINT
      Arquivo  : parser.c
      Função   : minha_funcao
      Linha    : 53
      Mensagem : Chegou aqui!
*********************************************************************
if (x > 10) {
    DEBUG_IF(x > 10);  // Só mostra se x > 10
}

    DEBUG CHECK POINT (condição verdadeira)
      Arquivo  : parser.c
      Função   : minha_funcao
      Linha    : 56
      Condição : x > 10
*********************************************************************

PARA DESABILITAR DEFINA: NDEBUG

********************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Macro principal de debug
#define DEBUG_CHECKPOINT() \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
    } while(0)

// Versão com mensagem personalizada
#define DEBUG_MSG(msg) \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
        printf("  %sMensagem%s : %s\n", "\033[1;33m", "\033[0m", msg); \
    } while(0)

// Versão com valor de variável
#define DEBUG_VAR(var) \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
        printf("  %sVariável%s : " #var " = %d\n", "\033[1;33m", "\033[0m", var); \
    } while(0)

// Versão para double
#define DEBUG_VAR_DBL(var) \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
        printf("  %sVariável%s : " #var " = %g\n", "\033[1;33m", "\033[0m", var); \
    } while(0)

// Versão para string
#define DEBUG_VAR_STR(var) \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
        printf("  %sVariável%s : " #var " = \"%s\"\n", "\033[1;33m", "\033[0m", var); \
    } while(0)

// Versão para ponteiro
#define DEBUG_VAR_PTR(var) \
    do { \
        printf("\n%sDEBUG CHECK POINT%s\n", "\033[1;36m", "\033[0m"); \
        printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
        printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
        printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
        printf("  %sVariável%s : " #var " = \"%p\"\n", "\033[1;33m", "\033[0m", var); \
    } while(0)

// Versão condicional (só mostra se condição for verdadeira)
#define DEBUG_IF(cond) \
    do { \
        if (cond) { \
            printf("\n%sDEBUG CHECK POINT (condição verdadeira)%s\n", "\033[1;36m", "\033[0m"); \
            printf("  %sArquivo%s  : %s\n", "\033[1;33m", "\033[0m", __FILE__); \
            printf("  %sFunção %s  : %s\n", "\033[1;33m", "\033[0m", __func__); \
            printf("  %sLinha  %s  : %d\n", "\033[1;33m", "\033[0m", __LINE__); \
            printf("  %sCondição%s : " #cond "\n", "\033[1;33m", "\033[0m"); \
        } \
    } while(0)

// Versão simples (sem cores) para quando cores não são suportadas
#define DEBUG_CHECKPOINT_SIMPLE() \
    do { \
        printf("\nDEBUG CHECK POINT\n"); \
        printf("  Arquivo : %s\n", __FILE__); \
        printf("  Função  : %s\n", __func__); \
        printf("  Linha   : %d\n", __LINE__); \
    } while(0)

// Para desabilitar debug em compilação de produção
#ifdef NDEBUG
    #define DEBUG_CHECKPOINT()          ((void)0)
    #define DEBUG_MSG(msg)              ((void)0)
    #define DEBUG_VAR(var)              ((void)0)
    #define DEBUG_VAR_DBL(var)          ((void)0)
    #define DEBUG_VAR_STR(var)          ((void)0)
    #define DEBUG_VAR_PTR(var)          ((void)0)
    #define DEBUG_IF(cond)              ((void)0)
    #define DEBUG_CHECKPOINT_SIMPLE()   ((void)0)
#endif

#endif // DEBUG_H