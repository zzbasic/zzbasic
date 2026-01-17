#include <stdio.h>

#include "utils.h"

// Configuração UTF-8 para Windows
#ifdef _WIN32
#include <windows.h>
void setup_utf8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
#else
void setup_utf8() {
    // No Linux/macOS, UTF-8 já é o padrão
}
#endif

void wait() {
    printf("\n""Pressione ENTER para continuar...");
    getchar(); // Espera o usuário pressionar Enter
    printf("\n");
}