#include <string.h>

#include "zzbasic.h"
#include "a89alloc.h"
#include "color.h"  

// ============================================
// Detecção do Sistema Operacional
// ============================================

static const char* get_os_name(void)
{
    #ifdef _WIN32
        return "Win32";
    #elif __linux__
        return "Linux";
    #elif __APPLE__
        return "macOS";
    #elif __unix__
        return "Unix";
    #else
        return "Unknown OS";
    #endif
}

// ============================================
// Função Principal
// ============================================
int main(int argc, char* argv[])
{
    print_banner();
    
    if (argc == 1)
    {
        // Modo REPL: nenhum argumento
        printf("Enter \"help\", a statement or \"exit\" to quit.\n\n");
        run_repl();
    }
    else if (argc == 2)
    {
        // Modo arquivo: um argumento (nome do arquivo)
        printf("ZzBasic v%s on %s\n\n", ZZ_VERSION, get_os_name());
        printf("Executing: %s\n", argv[1]);
        run_file(argv[1]);
    }
    else
    {
        // Muitos argumentos
        printf("Usage: zzbasic [file.zz]\n");
        printf("  No arguments: starts REPL\n");
        printf("  With filename: executes script\n");
        return 1;
    }
    
    return 0;
}

// ============================================
// Implementação do REPL
// ============================================
void run_repl(void)
{
    char linha[256];
    
    while (1)
    {
        printf(ZZ_PROMPT);
        
        // Lê uma linha do usuário
        if (fgets(linha, sizeof(linha), stdin) == NULL)
        {
            printf("\n");  // Nova linha após Ctrl+Z/D
            break;
        }
        
        // Remove o newline no final
        linha[strcspn(linha, "\n")] = '\0';
        
        // Comando para sair do REPL
        if (strcmp(linha, "exit") == 0 || strcmp(linha, "quit") == 0)
        {
            break;
        }
        
        // Comando de ajuda (placeholder para futuro)
        if (strcmp(linha, "help") == 0 || strcmp(linha, "?") == 0)
        {
            printf("ZzBasic commands:\n");
            printf("  exit, quit  - Exit REPL\n");
            printf("  help, ?     - Show this help\n");
            printf("\n");
            continue;
        }
        
        // VERSÃO 0.1.0: para testar o lexer
        if (strlen(linha) > 0)
        {
            printf("Tokens: ");
            lexer_print_all_tokens(linha);
        }
    }
}

// ============================================
// Implementação da Execução de Arquivo
// ============================================
void run_file(const char* filename)
{
    FILE* arquivo = fopen(filename, "r");
    if (!arquivo)
    {
        printf(COLOR_ERROR "Error: Cannot open file '%s'\n" COLOR_RESET, filename);
        return;
    }
    
    char linha[256];
    printf(CYAN "=== File content ===\n" COLOR_RESET);
    
    // VERSÃO 0.0.0: apenas exibe o conteúdo do arquivo
    while (fgets(linha, sizeof(linha), arquivo))
    {
        printf("%s", linha);
    }
    
    fclose(arquivo);
    printf(CYAN "\n=== End of file ===\n" COLOR_RESET);
}

// ============================================
// Banner de Inicialização (com ASCII art)
// ============================================
void print_banner(void)
{
    printf("\n");
    
    // ASCII art do ZzBasic (cor COLOR_SUCCESS - verde brilhante)
    printf("====================================\n");
    printf(COLOR_SUCCESS);
    printf(" ______    ____            _      \n");
    printf("|___  /   |  _ \\          (_)     \n");
    printf("   / / ___| |_) | __ _ ___ _  ___ \n");
    printf("  / / |_  /  _ < / _` / __| |/ __|\n");
    printf(" / /__ / /| |_) | (_| \\__ \\ | (__ \n");
    printf("/_____/___|____/ \\__,_|___/_|\\___|\n");
    printf(COLOR_RESET);
    printf("\nv%s on %s\n", ZZ_VERSION, get_os_name());
    printf("====================================\n");
    
    // Linhas e informação da versão/SO
    printf("\n");
}