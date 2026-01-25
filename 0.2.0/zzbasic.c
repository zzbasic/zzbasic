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
            printf("  exit, quit  - Exit REPL\n");
            printf("  help, ?     - Show this help\n");
            printf("\n");
            continue;
        }
        
        // VERSION 0.2.0: Use execute_statement with symbol table
        if (line[0] != '\0' && !is_empty_line(line))
        {
            Lexer lexer;
            lexer_init(&lexer, line);
    
            ASTNode* ast = parse_single_statement(&lexer);
            if (ast == NULL) {
                // Error already printed by parser
                continue;
            }
            
            int success = execute_statement(ast, symbols);
            if (!success) {
                // Error already printed by execute_statement
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

void run_file(const char* filename)
{
    size_t input_size;
    char* code = read_file(filename, &input_size);
    SymbolTable* symbols = symbol_table_create();

    if (code[0] != '\0')
    {
        Lexer lexer;
        lexer_init(&lexer, code);

        ASTNode* ast = parse_single_statement(&lexer);
        if (ast == NULL) {
            printf("%sParsing error%s\n", COLOR_ERROR, COLOR_RESET);
            symbol_table_destroy(symbols);
            a89free(code);
            exit(EXIT_FAILURE);
        }
        
        int success = execute_statement(ast, symbols);
        if (!success) {
            // Error already printed by execute_statement
        }
        
        free_ast(ast);
        a89free(code);
    }
    
    symbol_table_destroy(symbols);
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