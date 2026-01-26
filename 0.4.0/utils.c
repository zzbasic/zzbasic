// utils.c
#include <stdio.h>

#include "utils.h"

// UTF-8 setup for Windows
#ifdef _WIN32
#include <windows.h>
void setup_utf8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
#else
void setup_utf8() {
    // On Linux/macOS, UTF-8 is already the default
}
#endif

void wait() {
    printf("\n""Press ENTER to continue...");
    getchar(); // Wait for user to press Enter
    printf("\n");
}