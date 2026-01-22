#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include "color.h"  
#include "utils.h"
#include "zzbasic.h"
#include "a89alloc.h"
#include "ast.h"
#include "parser.h"
#include "evaluator.h"


// ============================================
// Detecção do Sistema Operacional
// ============================================
const char* get_os_name(void)
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

int is_empty_line(const char *line)
{
    if(line == NULL || *line == '\0')
    {
        return 1;
    }

    while(*line != '\0') {
        if(!isspace(*line)) {
            return 0;
        }
        line++;
    }
    
    return 1;
}


//===================================================================
// Lê o arquivo fonte todo e retorna como uma string
//
// O CHAMADOR É RESPONSÁVEL POR LIBERAR A MEMÓRIA ALOCADA
//
//===================================================================
char* read_file(const char* filename, size_t* length) {
    FILE* file = fopen(filename, "rb");
    if (!file){
        printf("Error opening the '%s' file: %s\n", 
               filename, strerror(errno));
        exit(EXIT_FAILURE);
    } 
    
    // Obtém tamanho do arquivo
    struct stat st;
    if (fstat(fileno(file), &st) != 0) {
        printf("Error retrieving %s file size: %s\n",
               filename, strerror(errno));
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    *length = st.st_size;
    
    // Aloca buffer (com espaço extra para null terminator se necessário)
    char* buffer = A89ALLOC(*length + 1);
    if (!buffer) {
        printf("Error allocating memory for the '%s' file\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    // Lê tudo de uma vez
    size_t bytes_read = fread(buffer, 1, *length, file);
    fclose(file);
    
    if (bytes_read != *length) {
        printf("Error reading '%s' file (expected %zu bytes, read %zu)\n",
               filename, *length, bytes_read);
        a89free(buffer);
        exit(EXIT_FAILURE);
    }
    
    buffer[*length] = '\0'; // Fecha a string
    return buffer;
}

// ============================================
// Implementação do REPL
// ============================================
void run_repl(void)
{
    char line[BUFFER_SIZE];
    
    while (1)
    {
        printf(ZZ_PROMPT);
        
        // Lê uma line do usuário
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");  // Nova line após Ctrl+Z/D
            break;
        }
        
        // Remove o newline no final
        line[strcspn(line, "\n")] = '\0';
        
        // Comando para sair do REPL
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0)
        {
            break;
        }
        
        // Comando de ajuda (placeholder para futuro)
        if (strcmp(line, "help") == 0 || strcmp(line, "?") == 0)
        {
            printf("ZzBasic commands:\n");
            printf("  exit, quit  - Exit REPL\n");
            printf("  help, ?     - Show this help\n");
            printf("\n");
            continue;
        }
        
        // VERSÃO 0.1.0: 
        if (line[0] != '\0' && !is_empty_line(line))
        {
            
            Lexer lexer;
            lexer_init(&lexer, line);
    
            ASTNode* ast = parse(&lexer);
            if (ast == NULL) {
                //printf("%s AQUI - Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
                continue;
            }
            
            EvaluatorResult result = evaluate(ast);
    
            if(result.success)
            {
                printf("%.2f\n", result.value.number_value);
            }
            else
            {
                 printf("%s\n", result.error_message);
            }
            
            free_ast(ast);
        }
        else
        {
            continue; // line vazia, exibe prompt novamente
        }
    }
}

void run_file(const char* filename)
{
    size_t input_size;
    char* code = read_file(filename, &input_size);

    if (code[0] != '\0')
    {
        Lexer lexer;
        lexer_init(&lexer, code);

        ASTNode* ast = parse(&lexer);
        if (ast == NULL) {
            printf("%s Parsing error%s\n", COLOR_ERROR, COLOR_RESET);
            exit(EXIT_FAILURE);
        }
        
        EvaluatorResult result = evaluate(ast);

        if(result.success)
        {
            printf("%.2f\n", result.value.number_value);
        }
        else
        {
            printf("%s\n", result.error_message);
        }
        
        free_ast(ast);
        a89free(code);
    }
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
