// zzbasic.c

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
// Operating System Detection
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
// Reads entire source file and returns as string
//
// CALLER IS RESPONSIBLE FOR FREEING THE ALLOCATED MEMORY
//
//===================================================================
char* read_file(const char* filename, size_t* length) {
    FILE* file = fopen(filename, "rb");
    if (!file){
        printf("Error opening file '%s': %s\n", 
               filename, strerror(errno));
        exit(EXIT_FAILURE);
    } 
    
    // Get file size
    struct stat st;
    if (fstat(fileno(file), &st) != 0) {
        printf("Error retrieving file size for '%s': %s\n",
               filename, strerror(errno));
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    *length = st.st_size;
    
    // Allocate buffer (with extra space for null terminator if needed)
    char* buffer = A89ALLOC(*length + 1);
    if (!buffer) {
        printf("Error allocating memory for file '%s'\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    // Read everything at once
    size_t bytes_read = fread(buffer, 1, *length, file);
    fclose(file);
    
    if (bytes_read != *length) {
        printf("Error reading file '%s' (expected %zu bytes, read %zu)\n",
               filename, *length, bytes_read);
        a89free(buffer);
        exit(EXIT_FAILURE);
    }
    
    buffer[*length] = '\0'; // Null-terminate string

    // ========== CORREÇÃO CRÍTICA PARA WINDOWS ==========
    // Remove todos os \r (carriage return)
    char* dst = buffer;
    char* src = buffer;
    size_t new_len = 0;
    
    while (*src) {
        if (*src != '\r') {  // Remove \r
            *dst++ = *src;
            new_len++;
        }
        src++;
    }
    *dst = '\0';
    *length = new_len;  // Atualiza tamanho
    // ===================================================

    return buffer;
}

// ============================================
// REPL Implementation
// ============================================
void run_repl(void)
{
    char line[BUFFER_SIZE];
    SymbolTable* symbols = symbol_table_create();
    
    while (1)
    {
        printf(ZZ_PROMPT);
        
        // Read a line from user
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");  // New line after Ctrl+Z/D
            break;
        }
        
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        
        // Command to exit REPL
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0)
        {
            break;
        }
        
        // Help command (placeholder for future)
        if (strcmp(line, "help") == 0 || strcmp(line, "?") == 0)
        {
            printf("ZzBasic commands:\n");
            printf("  exit, quit       - Exit REPL\n");
            printf("  help, ?          - Show this help\n");
            printf("  vars             - Show all variables\n");
            printf("  reset            - Clear all variables\n");
            printf("  clear            - Clear screen\n");
            printf("  tokens code      - Show tokens for code\n");
            printf("  ast code         - Show AST for code\n");
            printf("  symbols          - Show Symbol Table\n");
            printf("\nZzBasic language:\n");
            printf("  let var = expr   - Assign variable\n");
            printf("  expr1 : expr2    - Multiple statements\n");
            printf("  expr1 ; expr2    - Multiple statements\n");
            printf("\nExamples:\n");
            printf("  let x = 5 : x * 2\n");
            printf("  tokens x + 5 * 2\n");
            printf("  ast (a + b) * c\n");
            printf("\nEnter ZzBasic statements separated by : or ;\n");
            printf("Example: let x = 5 : let y = x * 2 : x + y\n");
            continue;
        }

        if (strcmp(line, "vars") == 0)
        {
            list_variables(symbols);
            continue;
        }

        if (strcmp(line, "reset") == 0)
        {
            printf("Resetting all variables...\n");
            symbol_table_destroy(symbols);
            symbols = symbol_table_create();  
            printf("All variables cleared.\n");
            continue;
        }

        if (strcmp(line, "clear") == 0)
        {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            printf("ZzBasic \nv%s on %s\n", ZZ_VERSION, get_os_name());
            continue;
        }

        if (strncmp(line, "tokens ", 7) == 0)
        {
            const char* code = line + 7;  // Pula "tokens "
            if (code[0] == '\0')
            {
                printf("Usage: tokens \"code\"\n");
                printf("Example: tokens \"x * 2\"\n");
                printf("         tokens \"let y = 10\"\n");
            }
            else
            {
                show_tokens(code);
            }
            continue;
        }

        if (strncmp(line, "ast ", 4) == 0)
        {
            const char* code = line + 4;  // Pula "ast "
            if (code[0] == '\0')
            {
                printf("Usage: ast \"code\"\n");
                printf("Example: ast \"x * 2\"\n");
                printf("         ast \"let y = (5 + 3) * 2\"\n");
            }
            else
            {
                show_ast(code);
            }
            continue;
        }

        if (strncmp(line, "symbols", 7) == 0)
        {
            if (symbols)
            {  
                symbol_table_print(symbols);
            }
            else
            {
                printf("Symbol table not initialized\n");
            }
            continue;
        }

        
        if (line[0] != '\0' && !is_empty_line(line))
        {
            Lexer lexer;
            lexer_init(&lexer, line);
    
            ASTNode* ast = parse(&lexer);  // Agora parse retorna statement list
            if (ast == NULL) {
                // Error already printed by parser
                continue;
            }
            
            int success = evaluate_program(ast, symbols);
            if (!success) {
                // Error already printed by evaluate_program
            }
            
            free_ast(ast);
        }
        else
        {
            continue; // Empty line, show prompt again
        }
    }
    
    symbol_table_destroy(symbols);
}


//=========== DEBUG ==============================
void debug_file(const char* filename) {
    size_t input_size;
    char* code = read_file(filename, &input_size);
    
    printf("=== DEBUG FILE ===\n");
    printf("Tamanho: %zu bytes\n", input_size);
    printf("Conteúdo (com códigos):\n");
    for (size_t i = 0; i < input_size; i++) {
        if (code[i] == '\n') printf("\\n");
        else if (code[i] == '\r') printf("\\r");
        else if (code[i] == '\t') printf("\\t");
        else if (code[i] == '\0') printf("\\0");
        else printf("%c", code[i]);
    }
    printf("\n=== END DEBUG ===\n\n");
    
    // Testa o lexer
    Lexer lexer;
    lexer_init(&lexer, code);
    
    printf("Tokens:\n");
    int count = 0;
    Token token;
    do
    {
        token = lexer_get_next_token(&lexer);
        printf("%3d: [%s] ", ++count, token_type_to_string(token.type));
        if (token.type == TOKEN_NUMBER) 
            printf("(%.2f)", token.value.number);
        else if (token.type == TOKEN_STRING) 
            printf("(%s)", token.value.string);  // Para strings
        else if (token.type == TOKEN_IDENTIFIER) 
            printf("(%s)", token.value.varname);  // Para identificadores
        else if (token.text[0]) 
            printf("(%s)", token.text);
        printf("\n");
    } while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
    
    a89free(code);
}
//================================================


void run_file(const char* filename)
{
    //debug_file(filename);

    size_t input_size;
    char* code = read_file(filename, &input_size);
    SymbolTable* symbols = symbol_table_create();

    if (code[0] != '\0')
    {
        Lexer lexer;
        lexer_init(&lexer, code);

        ASTNode* ast = parse(&lexer);// Agora parse retorna statement list
        if (ast == NULL) {
            printf("%sParsing error%s\n", COLOR_ERROR, COLOR_RESET);
            symbol_table_destroy(symbols);
            a89free(code);
            exit(EXIT_FAILURE);
        }

        int success = evaluate_program(ast, symbols);
        if (!success) {
            // Error already printed by execute_statement
        }

        // if (!success) {
        // printf("%sErro na execução%s\n", COLOR_ERROR, COLOR_RESET);
        // } else {
        //     printf("%sPrograma executado com sucesso!%s\n", COLOR_SUCCESS, COLOR_RESET);
        // }
        
        free_ast(ast);
        a89free(code);
    }
    
    symbol_table_destroy(symbols);
}

// ============================================
// Função para verificar extensão .zz
// ============================================
int has_zz_extension(const char* filename)
{
    if (!filename || strlen(filename) < 3) {
        return 0;  // Muito curto para ter ".zz"
    }
    
    // Encontra o último ponto
    const char* dot = strrchr(filename, '.');
    if (!dot) {
        return 0;  // Sem extensão
    }
    
    // Verifica se termina com ".zz" (case-insensitive)
    size_t len = strlen(dot);
    if (len != 3) return 0;  // ".zz" tem 3 caracteres
    
    // Converte para minúsculas para comparação
    // char ext[4];
    // ext[0] = tolower(dot[1]);
    // ext[1] = tolower(dot[2]);
    // ext[2] = tolower(dot[3] ? dot[3] : ' ');  // Segurança
    // ext[3] = '\0';
    
    return (strcmp(dot, ".zz") == 0);
}

void list_variables(SymbolTable* symbols)
{
    if (!symbols) {
        printf("Error: no symbol table available\n");
        return;
    }
    
    int count = symbol_table_count(symbols);
    if (count == 0) {
        printf("No variables defined.\n");
        return;
    }
    
    // Usa a função existente - mostra cabeçalho e separadores
    // que são úteis no REPL também
    symbol_table_print(symbols);
}

void show_tokens(const char* code)
{
    lexer_print_all_tokens(code);
}

void show_ast(const char* code)
{
    Lexer lexer;
    lexer_init(&lexer, code);
    
    ASTNode* ast = parse(&lexer);
    if (!ast) {
        printf("Error parsing code.\n");
        return;
    }
    
    printf("%sAST for: \"%s\"\n%s", COLOR_HEADER, code, COLOR_RESET);
    print_ast(ast, 0);
    
    free_ast(ast);
}


// ============================================
// Initialization Banner (with ASCII art)
// ============================================
void print_banner(void)
{
    printf("\n");
    
    // ZzBasic ASCII art (COLOR_SUCCESS - bright green)
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
    
    // Lines and version/OS information
    printf("\n");
}
// Fim de zzbasic.c