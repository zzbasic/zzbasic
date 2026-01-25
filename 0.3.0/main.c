#include <string.h>

#include "color.h"
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
        print_banner();
        printf("Enter \"help\", a statement or \"exit\" to quit.\n\n");
        run_repl();
    }
    else if (argc == 2)
    {
        // Modo arquivo: um argumento
        char* filename = argv[1];
        
        // Verifica se tem extensão .zz
        if (!has_zz_extension(filename)) {
            printf("%sError: ZzBasic scripts must have .zz extension\n", 
                   COLOR_ERROR);
            printf("Received: '%s'\n", filename);
            printf("Expected: 'program.zz'\n");
            printf("Example: zzbasic programa.zz%s\n", COLOR_RESET);
            return 1;
        }
        
        // Tenta abrir o arquivo para verificar existência
        FILE* test = fopen(filename, "r");
        if (!test) {
            printf("%sError: cannot open file '%s'%s\n", 
                   COLOR_ERROR, filename, COLOR_RESET);
            printf("Check if file exists and you have read permission.\n");
            return 1;
        }
        fclose(test);
        
        printf("ZzBasic v%s on %s\n", ZZ_VERSION, get_os_name());
        printf("Executing: %s\n", filename);
        run_file(filename);  
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