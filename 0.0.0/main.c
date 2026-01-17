#include "zzbasic.h"
//#include "a89alloc.h"  // Sua biblioteca de alocação
#include <string.h>

// ================================
// Função Principal
// ================================
int main(int argc, char* argv[]) {
    setup_utf8();
    print_banner();
    
    if (argc == 1) {
        // Modo REPL: nenhum argumento
        printf("Modo REPL ativado. Digite 'sair' para encerrar.\n");
        run_repl();
    } else if (argc == 2) {
        // Modo arquivo: um argumento (nome do arquivo)
        printf("Executando arquivo: %s\n", argv[1]);
        run_file(argv[1]);
    } else {
        // Muitos argumentos
        printf("Uso: zzbasic [arquivo.zz]\n");
        printf("  Sem argumentos: inicia o REPL\n");
        printf("  Com arquivo: executa o script\n");
        return 1;
    }
    
    return 0;
}

// ================================
// Implementação do REPL
// ================================
void run_repl(void) {
    char linha[256];  // Buffer para a linha de entrada
    
    while (1) {
        printf(ZZ_PROMPT);
        
        // Lê uma linha do usuário
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\n");  // Nova linha após Ctrl+Z/D
            break;
        }
        
        // Remove o newline no final
        linha[strcspn(linha, "\n")] = '\0';
        
        // Comando para sair do REPL
        if (strcmp(linha, "quit") == 0 || strcmp(linha, "exit") == 0) {
            printf("Encerrando ZzBasic. Até logo!\n");
            break;
        }
        
        // VERSÃO 0.0.0: apenas ecoa o que foi digitado
        printf("Eco: \"%s\"\n", linha);
    }
}

// ================================
// Implementação da Execução de Arquivo
// ================================
void run_file(const char* filename) {
    FILE* arquivo = fopen(filename, "r");
    if (!arquivo) {
        printf("Erro: Não foi possível abrir o arquivo '%s'\n", filename);
        return;
    }
    
    char linha[256];
    printf("=== Conteúdo do arquivo ===\n");
    
    // VERSÃO 0.0.0: apenas exibe o conteúdo do arquivo
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }
    
    fclose(arquivo);
    printf("=== Fim do arquivo ===\n");
}

// ================================
// Banner de Inicialização
// ================================
void print_banner(void) {
    printf("\n");
    printf("========================================\n");
    printf("        ZzBasic v%s\n", ZZ_VERSION);
    printf("========================================\n");
    printf("\n");
}