#include <stdio.h>
#include <string.h>
#include "color.h"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

// Função auxiliar: aguarda Enter ou ESC
static int wait_for_enter(void)
{
#ifdef _WIN32
    /* Windows: usar conio.h */
    printf("Press ENTER to continue or ESC to exit help...");
    fflush(stdout);
    
    while (1)
    {
        int ch = _getch();  /* Lê uma tecla sem ecoar */
        
        if (ch == 27)  /* ESC */
        {
            printf("\n");
            return 1;  /* Sair do help */
        }
        else if (ch == '\r' || ch == '\n')  /* Enter */
        {
            printf("\n");
            return 0;  /* Continuar */
        }
    }
#else
    /* Linux/Mac: usar termios */
    struct termios old_term, new_term;
    unsigned char ch;
    int result = 0;
    
    printf("Press ENTER to continue or ESC to exit help...");
    fflush(stdout);
    
    /* Salvar configurações originais */
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    
    /* Modo raw: desabilita canonização */
    new_term.c_lflag &= ~(ICANON | ECHO);
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    
    while (1)
    {
        if (read(STDIN_FILENO, &ch, 1) == 1)
        {
            if (ch == 27)  /* ESC */
            {
                result = 1;  /* Sair do help */
                break;
            }
            else if (ch == '\n' || ch == '\r')  /* Enter */
            {
                result = 0;  /* Continuar */
                break;
            }
        }
    }
    
    /* Restaurar configurações originais */
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    printf("\n");
    
    return result;
#endif
}

// Função auxiliar: exibe uma página
static void print_page(const char* content)
{
    printf("%s", content);
    fflush(stdout);
}

// Função principal: exibe help com paginação
void display_help(void)
{
    // Página 1: Comandos REPL
    print_page(
        CYAN "ZzBasic v0.7.0 - Help\n"
        "=====================" RESET "\n"
        "\n"
        CYAN "REPL Commands:" RESET "\n"
        "  " GREEN "exit, quit" RESET "       - Exit REPL\n"
        "  " GREEN "help, ?" RESET "          - Show this help\n"
        "  " GREEN "vars" RESET "             - Show all variables\n"
        "  " GREEN "reset" RESET "            - Clear all variables\n"
        "  " GREEN "clear" RESET "            - Clear screen\n"
        "  " GREEN "tokens" RESET " code      - Show tokens for code\n"
        "  " GREEN "ast" RESET " code         - Show AST for code\n"
        "  " GREEN "symbols" RESET "          - Show Symbol Table\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 2: Comandos de Linguagem
    print_page(
        "\n"
        CYAN "ZzBasic Language Commands:" RESET "\n"
        "  " GREEN "let" RESET " var = expr   - Assign variable\n"
        "  expr1 " GREEN ":" RESET " expr2    - Multiple statements\n"
        "  expr1 " GREEN ";" RESET " expr2    - Multiple statements\n"
        "  " GREEN "print" RESET " expr       - Print values\n"
        "  " GREEN "input" RESET " prompt var - Read input\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 3: Controle de Fluxo
    print_page(
        "\n"
        CYAN "Control Flow - if/then/else:" RESET "\n"
        "  " GREEN "if" RESET " (condition) " GREEN "then" RESET "\n"
        "      statements\n"
        "  " GREEN "else if" RESET " (condition) " GREEN "then" RESET "\n"
        "      statements\n"
        "  " GREEN "else" RESET "\n"
        "      statements\n"
        "  " GREEN "end if" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 4: Loops - While
    print_page(
        "\n"
        CYAN "Control Flow - while loops:" RESET "\n"
        "  " GREEN "while" RESET " (condition) " GREEN "do" RESET "\n"
        "      statements\n"
        "      " GREEN "break" RESET "        - Exit loop\n"
        "      " GREEN "continue" RESET "     - Skip to next iteration\n"
        "  " GREEN "end while" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 5: Loops - For
    print_page(
        "\n"
        CYAN "Control Flow - for loops:" RESET "\n"
        "  " GREEN "for" RESET " var = start " GREEN "to" RESET " end [" GREEN "step" RESET " n] " GREEN "do" RESET "\n"
        "      statements\n"
        "      " GREEN "break" RESET "        - Exit loop\n"
        "      " GREEN "continue" RESET "     - Skip to next iteration\n"
        "  " GREEN "end for" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 6: Operadores
    print_page(
        "\n"
        CYAN "Operators:" RESET "\n"
        "  " YELLOW "Arithmetic:" RESET "  +, -, *, /, %\n"
        "  " YELLOW "Comparison:" RESET "  ==, !=, <, >, <=, >=\n"
        "  " YELLOW "Logical:" RESET "     " GREEN "and" RESET ", " GREEN "or" RESET ", " GREEN "not" RESET ", !\n"
        "\n"
        CYAN "Precedence (highest to lowest):" RESET "\n"
        "  1. Parentheses ()\n"
        "  2. Unary: +, -, " GREEN "not" RESET ", !\n"
        "  3. Multiplication, Division, Modulo: *, /, %\n"
        "  4. Addition, Subtraction: +, -\n"
        "  5. Comparison: ==, !=, <, >, <=, >=\n"
        "  6. AND: " GREEN "and" RESET "\n"
        "  7. OR: " GREEN "or" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 7: Print e Formatação
    print_page(
        "\n"
        CYAN "Print Command:" RESET "\n"
        "  " GREEN "print" RESET " expr1 expr2 ... exprN " GREEN "nl" RESET "\n"
        "\n"
        GREEN "nl:" RESET " forces a new line in print statements.\n"
        "\n"
        CYAN "Colors:" RESET "\n"
        "  red, green, yellow, blue, magenta, cyan, white\n"
        "  bred, bgreen, byellow, bblue, bmagenta, bcyan, bwhite\n"
        "  bgred, bggreen, bgyellow, bgblue, bgmagenta, bgcyan, bgwhite\n"
        "  " GREEN "nocolor" RESET " (reset to default)\n"
        "\n"
        CYAN "Formatting:" RESET "\n"
        "  " GREEN "width(n)" RESET "  - Set field width\n"
        "  " GREEN "left" RESET "      - Align left\n"
        "  " GREEN "right" RESET "     - Align right\n"
        "  " GREEN "center" RESET "    - Align center\n"
        "\n"
        "  Example:\n"
        "  " GREEN "print" RESET " red " GREEN "width" RESET "(20) " GREEN "right" RESET " expr " GREEN "nocolor" RESET " " GREEN "nl" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
 
    // Página 8: input
    print_page(
        "\n"
        CYAN "Input Command:" RESET "\n"
        "  " GREEN "input" RESET " [formatting] \"prompt\" variable\n"
        "\n"
        "The input prompt accepts colors, width and alignment, similar to " GREEN "print" RESET ".\n"
    );

    // Página 9: Comentários
    print_page(
        "\n"
        CYAN "Comments:" RESET "\n"
        "  " GREEN "#" RESET " Single-line comment\n"
        "    Everything after " GREEN "#" RESET " until end of line is ignored\n"
        "\n"
        "  " GREEN "\"\"\"" RESET " Multi-line comment\n"
        "      Can span multiple lines\n"
        "      Everything between " GREEN "\"\"\"" RESET " markers is ignored\n"
        "  " GREEN "\"\"\"" RESET "\n"
        "\n"
    );
    if (wait_for_enter()) return;
    
    // Página 10: Arrays
    print_page(
        "\n"
        CYAN "Arrays:" RESET "\n"                       
        "  " GREEN "let" RESET " arr = " GREEN "array" RESET "(5)         - Create array\n"
        "  arr[0]                     - Access element (0-indexed)\n"
        "  " GREEN "push" RESET "(arr, value)           - Add element to end\n"
        "  " GREEN "pop" RESET "(arr)                   - Remove last element\n"
        "  " GREEN "len" RESET "(arr)                   - Get array length\n"
        "  " GREEN "get" RESET "(arr, index)            - Gets an element at a specific index\n"
        "  " GREEN "set" RESET "(arr, index, value)     - Sets an element at a specific index\n"
        "  " GREEN "insert" RESET "(arr, index, value)  - Inserts an element at a specific index\n"
        "  " GREEN "remove" RESET "(arr, index)         - Removes an element at a specific index\n"
        "  " GREEN "is_empty" RESET "(arr)              - Checks if the array is empty\n"
        "\n"
    );
    if (wait_for_enter()) return;
}
