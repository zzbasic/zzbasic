// zzbasic.c

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include "help.h"  
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
        //exit(EXIT_FAILURE);
        return NULL;
    } 
    
    // Get file size
    struct stat st;
    if (fstat(fileno(file), &st) != 0) {
        printf("Error retrieving file size for '%s': %s\n",
               filename, strerror(errno));
        fclose(file);
        //exit(EXIT_FAILURE);
        return NULL;
    }
    
    *length = st.st_size;
    
    // Allocate buffer (with extra space for null terminator if needed)
    char* buffer = A89ALLOC(*length + 1);
    if (!buffer) {
        printf("Error allocating memory for file '%s'\n", filename);
        fclose(file);
        //exit(EXIT_FAILURE);
        return NULL;
    }
    
    // Read everything at once
    size_t bytes_read = fread(buffer, 1, *length, file);
    fclose(file);
    
    if (bytes_read != *length) {
        printf("Error reading file '%s' (expected %zu bytes, read %zu)\n",
               filename, *length, bytes_read);
        a89free(buffer);
        //exit(EXIT_FAILURE);
        return NULL;
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
// REPL Multi-line
// ============================================
ASTNode* compile_program(ReplProgram* program)
{
    if (program->line_count == 0)
    {
        return NULL;
    }

    if (program->ast)
    {
        free_ast(program->ast);
        program->ast = NULL;
    }
    
    // Calcula tamanho total necessário
    size_t total_size = 0;
    for (int i = 0; i < program->line_count; i++)
    {
        if (program->lines[i])
            total_size += strlen(program->lines[i]) + 1;  // +1 para \n
    }
    
    if (total_size == 0)
        return NULL;
    
    // Aloca buffer com tamanho exato
    char* full_code = A89ALLOC(total_size + 1);  // +1 para \0
    if (!full_code)
        return NULL;
    
    full_code[0] = '\0';
    
    // Concatena todas as linhas
    for (int i = 0; i < program->line_count; i++)
    {
        if (program->lines[i])
        {
            strcat(full_code, program->lines[i]);
            strcat(full_code, "\n");
        }
    }
    
    // Parse
    Lexer lexer;
    lexer_init(&lexer, full_code);
    ASTNode* ast = parse(&lexer);
    
    a89free(full_code);

    program->ast = ast;  // Guarda para executar depois 
    
    return ast;
}

void parse_range(const char* arg, int max_lines, int* start, int* end)
{
    char* dash = strchr(arg, '-');
    
    if (dash == NULL)
    {
        // Apenas um número: "n"
        *start = *end = atoi(arg);
    }
    else if (dash == arg)
    {
        // "-m": do início até m
        *start = 1;
        *end = atoi(dash + 1);
    }
    else if (*(dash + 1) == '\0')
    {
        // "n-": de n até o fim
        *start = atoi(arg);
        *end = max_lines;
    }
    else
    {
        // "n-m": de n até m
        *start = atoi(arg);
        *end = atoi(dash + 1);
    }
}

// Processa input quando em modo program
static int program_mode_input(ReplProgram* program, const char* line)
{
    if (strcmp(line, "end program") == 0)
    {
        program->in_program_mode = 0;
        program->ast = compile_program(program);
        printf("%s[Program loaded successfully]%s\n", COLOR_SUCCESS, COLOR_RESET);
        return 1;  // Saiu do modo program
    }
    
    // Store line if not at max capacity
    if (program->line_count < MAX_PROGRAM_LINES)
    {
        strncpy(program->lines[program->line_count], line, PROGRAM_LINE_SIZE - 1);

        program->lines[program->line_count][PROGRAM_LINE_SIZE - 1] = '\0';
        program->line_count++;
    }
    else
    {
        printf("%s[Error: Maximum program lines (%d) reached]%s\n", 
               COLOR_ERROR, MAX_PROGRAM_LINES, COLOR_RESET);
    }
    
    return 0;  // Continua em modo program
}

// Comando 'program'. Entra no modo programa
static void program_command(ReplProgram* program)
{
    program->in_program_mode = 1;
    program->line_count = 0;
    printf("%s[PROGRAM MODE]%s\n", COLOR_HEADER, COLOR_RESET);
}

// Comando 'end program'. Sai do modo programa
static void end_program_command(ReplProgram* program)
{
    program->in_program_mode = 0;
    program->ast = compile_program(program);
    printf("%s[Program loaded successfully]%s\n", COLOR_SUCCESS, COLOR_RESET);
}

// Comando 'run'. Executa o programa 
static void run_command(ReplProgram* program, SymbolTable* symbols)
{
    if (program->line_count == 0)
    {
        printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
    }
    else if (program->ast)
    {
        evaluate_program(program->ast, symbols);
    }
    else
    {
        printf("%s[Error: Program not compiled]%s\n", COLOR_ERROR, COLOR_RESET);
    }
}

// Comando 'purge'. Limpa programa E variáveis da memória 
static void purge_command(ReplProgram* program, SymbolTable** symbols)
{
    // Limpa programa
    if (program->ast)
    {
        free_ast(program->ast);
        program->ast = NULL;
    }
    
    // Zera o array de linhas; Libera todas as linhas
    for (int i = 0; i < program->line_count; i++)
    {
        if (program->lines[i])
        {
            a89free(program->lines[i]);
            program->lines[i] = NULL;
        }
    }
    program->line_count = 0;
    
    // Limpa variáveis
    symbol_table_destroy(*symbols);
    *symbols = symbol_table_create();
    
    printf("%s[Memory purged]%s\n", COLOR_SUCCESS, COLOR_RESET);
}

// Comando 'list'. Lista todas as linhas
static void list_command(ReplProgram* program)
{
    if (program->line_count == 0)
    {
        printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
    }
    else
    {
        for (int i = 0; i < program->line_count; i++)
        {
            printf("%02d: %s\n", i + 1, program->lines[i]);
        }
    }
}

// Comando 'list' com opções
static void list_range_command(ReplProgram* program, const char* arg)
{
    if (program->line_count == 0)
    {
        printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
        return;
    }
    
    int start, end;
    parse_range(arg, program->line_count, &start, &end);
    
    // Valida o intervalo de linhas
    if (start < 1 || start > program->line_count || 
        end < 1 || end > program->line_count || start > end)
    {
        printf("Invalid range: %s\n", arg);
    }
    else
    {
        for (int i = start - 1; i < end; i++)
        {
            printf("%02d: %s\n", i + 1, program->lines[i]);
        }
    }
}

// Comando 'edit'. Edita linha
static void edit_command(ReplProgram* program, const char* arg)
{
    if (!arg || *arg == '\0')
    {
        printf("%sError: edit command requires line number%s\n", COLOR_ERROR, COLOR_RESET);
        return;
    }
    
    char* endptr;
    errno = 0;
    long n = strtol(arg, &endptr, 10);  // Base 10
    
    // Verificar erros
    if (endptr == arg)
    {
        printf("%sError: '%s' is not a valid number%s\n", COLOR_ERROR, arg, COLOR_RESET);
        return;
    }
    
    if (*endptr != '\0')
    {
        printf("%sWarning: extra characters after number: '%s'%s\n", COLOR_WARNING, endptr, COLOR_RESET);
        return;
    }
    
    if (errno == ERANGE)
    {
        printf("%sError: line number '%s' is out of range%s\n", COLOR_ERROR, arg, COLOR_RESET);
        return;
    }
    
    // Verificar limites (1-100)
    if (n < 1 || n > 99) {
        printf("%sError: line number must be between 1 and 99%s\n", COLOR_ERROR, COLOR_RESET);
        return;
    }
    

    int line_num = (int)n;
    
    if (program->line_count == 0)
    {
        printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
        return;
    }
    
    if (line_num < 1 || line_num > program->line_count)
    {
        printf("%sError: invalid line number: %d%s\n", COLOR_WARNING, line_num, COLOR_RESET);
        return;
    }
    
    printf("%02d: %s\n", line_num, program->lines[line_num - 1]);
    printf("%02d: ", line_num);
    fflush(stdout);
    
    char new_line[PROGRAM_LINE_SIZE];
    if (fgets(new_line, sizeof(new_line), stdin) != NULL)
    {
        new_line[strcspn(new_line, "\n")] = '\0';
        strncpy(program->lines[line_num - 1], new_line, PROGRAM_LINE_SIZE - 1);
        program->lines[line_num - 1][PROGRAM_LINE_SIZE - 1] = '\0';
    }
}

// Comando 'delete'
static void delete_command(ReplProgram* program, const char* arg)
{
    if (program->line_count == 0)
    {
        printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
        return;
    }
    
    int start, end;
    parse_range(arg, program->line_count, &start, &end);
    
    // Valida range
    if (start < 1 || start > program->line_count || 
        end < 1 || end > program->line_count || start > end)
    {
        printf("%sInvalid range: %s%s\n", COLOR_WARNING, arg, COLOR_RESET);
        return;
    }
    
    int lines_to_delete = end - start + 1;
    
    // 'deleta' as linhas (sobrescrevendo as linhas a serem deletadas)
    for (int i = start - 1; i < program->line_count - lines_to_delete; i++)
    {
        strcpy(program->lines[i], program->lines[i + lines_to_delete]);
    }
    
    program->line_count -= lines_to_delete;
}

// Comando 'tokens' 
static void tokens_command(ReplProgram* program, const char* code)
{
    if (strcmp(code, "program") == 0)
    {
        if (program->line_count == 0)
        {
            printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
            return;
        }
        
        // Concatenate all program lines and show tokens
        char* full_code = A89ALLOC(PROGRAM_LINE_SIZE * MAX_PROGRAM_LINES);
        if (full_code)
        {
            full_code[0] = '\0';
            for (int i = 0; i < program->line_count; i++)
            {
                strncat(full_code, program->lines[i], 
                        PROGRAM_LINE_SIZE * MAX_PROGRAM_LINES - strlen(full_code) - 1);

                strncat(full_code, "\n", 
                        PROGRAM_LINE_SIZE * MAX_PROGRAM_LINES - strlen(full_code) - 1);
            }
            show_tokens(full_code);
            a89free(full_code);
        }
    }
    else if (code[0] == '\0')
    {
        printf("Usage: tokens \"code\" or tokens program\n");
    }
    else
    {
        show_tokens(code);
    }
}

// Comando 'ast'
static void ast_command(ReplProgram* program, const char* code)
{
    if (strcmp(code, "program") == 0)
    {
        if (program->line_count == 0)
        {
            printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
            return;
        }
        
        if (program->ast)
        {
            printf("%sAST for program:%s\n", COLOR_HEADER, COLOR_RESET);
            print_ast(program->ast, 0);
        }
    }
    else if (code[0] == '\0')
    {
        printf("Usage: ast \"code\" or ast program\n");
    }
    else
    {
        show_ast(code);
    }
}

// Comando 'symbols'
static void symbols_command(ReplProgram* program, SymbolTable* symbols, const char* code)
{
    if (strcmp(code, " program") == 0)
    {
        if (program->line_count == 0)
        {
            printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
            return;
        }
        
        printf("%sSymbols in program:%s\n", COLOR_HEADER, COLOR_RESET);
        symbol_table_print(symbols);
    }
    else if (symbols)
    {  
        symbol_table_print(symbols);
    }
    else
    {
        printf("Symbol table not initialized\n");
    }
}

// Comando 'vars'
static void vars_command(ReplProgram* program, SymbolTable* symbols, const char* code)
{
    if (strcmp(code, " program") == 0)
    {
        if (program->line_count == 0)
        {
            printf("%s[No program in memory]%s\n", COLOR_WARNING, COLOR_RESET);
            return;
        }
        
        printf("%sVariables in program:%s\n", COLOR_HEADER, COLOR_RESET);
        //symbol_table_print(symbols);
        list_variables(symbols);
    }
    else
    {
        list_variables(symbols);
    }
}

static void execute_code(const char* line, SymbolTable* symbols)
{
    if (line[0] == '\0' || is_empty_line(line))
    {
        return;  
    }
    
    Lexer lexer;
    lexer_init(&lexer, line);
    
    ASTNode* ast = parse(&lexer);
    if (ast == NULL) {
        // Error already printed by parser
        return;
    }

    evaluate_program(ast, symbols);
    free_ast(ast);
}

void run_repl(void)
{
    char line[BUFFER_SIZE];
    SymbolTable* symbols = symbol_table_create();
    ReplProgram program = {0};
    
    while (1)
    {
        // Determine prompt based on mode
        if (program.in_program_mode)
        {
            printf("%02d: ", program.line_count + 1);
        }
        else
        {
            printf(ZZ_PROMPT);
        }
        
        fflush(stdout);
        
        // Read a line from user
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");
            break;
        }
        
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        
        // If in program mode
        if (program.in_program_mode)
        {
            if (program_mode_input(&program, line))
            {
                // Saiu do modo program
            }
            continue;
        }
        
        // Normal mode: process commands
        
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0)
            break;
        
        if (strcmp(line, "help") == 0 || strcmp(line, "?") == 0)
        {
            display_help();
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

        if (strcmp(line, "program") == 0)
        {
            program_command(&program);
            continue;
        }

        if (strcmp(line, "run") == 0)
        {
            run_command(&program, symbols);
            continue;
        }

        if (strcmp(line, "purge") == 0)
        {
            purge_command(&program, &symbols);
            continue;
        }

        if (strcmp(line, "list") == 0)
        {
            list_command(&program);
            continue;
        }

        // >> list 2-5
        // line + 5 aponta para o inicio do argumento, o char 2
        if (strncmp(line, "list ", 5) == 0)
        {
            list_range_command(&program, line + 5);
            continue;
        }

        if (strncmp(line, "edit ", 5) == 0)
        {
            edit_command(&program, line + 5);
            continue;
        }

        if (strncmp(line, "delete ", 7) == 0)
        {
            delete_command(&program, line + 7);
            continue;
        }

        if (strncmp(line, "tokens ", 7) == 0)
        {
            tokens_command(&program, line + 7);
            continue;
        }

        if (strncmp(line, "ast ", 4) == 0)
        {
            ast_command(&program, line + 4);
            continue;
        }

        if (strncmp(line, "symbols", 7) == 0)
        {
            symbols_command(&program, symbols, line + 7);
            continue;
        }

        if (strncmp(line, "vars ", 5) == 0)
        {
            vars_command(&program, symbols, line + 5);
            continue;
        }

        // Execute as code
        execute_code(line, symbols);
    }
    
    // Cleanup
    if (program.ast)
    {
        free_ast(program.ast);
    }
    symbol_table_destroy(symbols);
}


void run_file(const char* filename)
{
    size_t input_size;
    char* code = read_file(filename, &input_size);

    if (!code) {
        printf("%sError: could not read file '%s'%s\n", 
               COLOR_ERROR, filename, COLOR_RESET);
        return;
    }

    SymbolTable* symbols = symbol_table_create();

    if (code[0] != '\0')
    {
        Lexer lexer;
        lexer_init(&lexer, code);

        ASTNode* ast = parse(&lexer);// Agora parse retorna statement list
        if (ast == NULL) {
            printf("%sParsing error: ast == NULL%s\n", COLOR_ERROR, COLOR_RESET);
            symbol_table_destroy(symbols);
            a89free(code);
            exit(EXIT_FAILURE);
        }

        evaluate_program(ast, symbols);
        
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