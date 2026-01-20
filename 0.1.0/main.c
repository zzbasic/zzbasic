#include <string.h>

#include "utils.h"
#include "zzbasic.h"
#include "a89alloc.h"

// ============================================
// Função Principal
// ============================================
int main(int argc, char* argv[])
{
    setup_utf8();
    
    if (argc == 1)
    {
        // Modo REPL: nenhum argumento
        printf("Enter \"help\", a statement or \"exit\" to quit.\n\n");
        print_banner();
        run_repl();
    }
    else if (argc == 2)
    {
        // Modo arquivo: um argumento (nome do arquivo)
        printf("ZzBasic v%s on %s\n", ZZ_VERSION, get_os_name());
        printf("Executing: %s\n", argv[1]);
        run_file(argv[1]);  
    }
    else
    {
        // Mais de um argumento - ERRO
        printf("Usage: zzbasic [file.zz]\n");
        printf("  No arguments: starts REPL\n");
        printf("  With filename: executes script\n");
        return 1;
    }

    a89check_leaks();
    return 0;
}