#include <stdio.h>
#include <string.h>
#include "help.h"

// Função auxiliar: aguarda Enter
static void wait_for_enter(void)
{
    printf("Press ENTER to continue...");
    fflush(stdout);
    while (getchar() != '\n');
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
        "ZzBasic v0.5.2 - Help\n"
        "=====================\n"
        "\n"
        "REPL Commands:\n"
        "  exit, quit       - Exit REPL\n"
        "  help, ?          - Show this help\n"
        "  vars             - Show all variables\n"
        "  reset            - Clear all variables\n"
        "  clear            - Clear screen\n"
        "  tokens code      - Show tokens for code\n"
        "  ast code         - Show AST for code\n"
        "  symbols          - Show Symbol Table\n"
        "\n"
    );
    wait_for_enter();
    
    // Página 2: Comandos de Linguagem
    print_page(
        "\n"
        "ZzBasic Language Commands:\n"
        "  let var = expr   - Assign variable\n"
        "  expr1 : expr2    - Multiple statements\n"
        "  expr1 ; expr2    - Multiple statements\n"
        "  print expr       - Print values\n"
        "  input msg var    - Read input\n"
        "\n"
    );
    wait_for_enter();
    
    // Página 3: Controle de Fluxo
    print_page(
        "\n"
        "Control Flow:\n"
        "  if (condition) then\n"
        "      statements\n"
        "  else if (condition) then\n"
        "      statements\n"
        "  else\n"
        "      statements\n"
        "  end if\n"
        "\n"
        "Note: Use 'nl' to go to next line in REPL:\n"
        "  >> if(n == 3) then nl print \"n é 3\" nl end if\n"
        "\n"
    );
    wait_for_enter();
    
    // Página 4: Operadores
    print_page(
        "\n"
        "Operators:\n"
        "  Arithmetic:  +, -, *, /\n"
        "  Comparison:  ==, !=, <, >, <=, >=\n"
        "  Logical:     and, or, not, !\n"
        "\n"
        "Precedence (highest to lowest):\n"
        "  1. Parentheses ()\n"
        "  2. Unary: +, -, not, !\n"
        "  3. Multiplication, Division: *, /\n"
        "  4. Addition, Subtraction: +, -\n"
        "  5. Comparison: ==, !=, <, >, <=, >=\n"
        "  6. AND: and\n"
        "  7. OR: or\n"
        "\n"
    );
    wait_for_enter();
    
    // Página 5: Print e Formatação
    print_page(
        "\n"
        "Print Command:\n"
        "  print expr1 expr2 ... exprN nl\n"
        "\n"
        "nl: forces a new line in print statements.\n"
        "\n"
        "Colors:\n"
        "  red, green, yellow, blue, magenta, cyan, white\n"
        "  bred, bgreen, byellow, bblue, bmagenta, bcyan, bwhite\n"
        "  bgred, bggreen, bgyellow, bgblue, bgmagenta, bgcyan, bgwhite\n"
        "  nocolor (reset to default)\n"
        "\n"
        "Formatting:\n"
        "  width(n)  - Set field width\n"
        "  left      - Align left\n"
        "  right     - Align right\n"
        "  center    - Align center\n"
        "\n"
    );
    wait_for_enter();
    
    // Página 6: Exemplos
    print_page(
        "\n"
        "Examples:\n"
        "  let x = 5 : x * 2\n"
        "  tokens x + 5 * 2\n"
        "  ast (a + b) * c\n"
        "  print \"Result:\" 10 + 5 nl\n"
        "  print red width(40) center \"SECURITY ALERT\" nocolor nl\n"
        "  print yellow width(40) center \"Check system permissions\" nocolor nl\n"
        "  input \"Name: \" name\n"
        "  if (x > 10) then nl print \"large\" nl end if\n"
        "\n"
        "Help completed.\n"
        "\n"
    );
}