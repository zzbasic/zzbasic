#include <stdio.h>

#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "a89alloc.h"

int main()
{
	setup_utf8();
	char expr[][100] =
    {
	    "-5 + 3",
	    "10 - 4 * 2",
	    "-(5 + 3) * 2",
	    "10 / 2 + 1",
	};
	int size = sizeof(expr) / sizeof(expr[0]);   
    for (int i = 0; i < size; i++)
    {
    	Lexer lexer;
    	Token token;  
        lexer_init(&lexer, expr[i]);
		printf("%sExpressão: %s%s\n",CYAN, expr[i], COLOR_RESET);
        ASTNode* ast = parse(&lexer);
        if (ast != NULL)
        {
            printf(COLOR_SUCCESS "Parsing bem-sucedido!\n" COLOR_RESET);
            printf("Árvore Sintática (AST):\n");
            print_ast(ast, 1);
            free_ast(ast);
        }
        else
        {
            printf(COLOR_ERROR "Erro no parsing\n" COLOR_RESET);
        }
		wait();
	}

    a89check_leaks();
	return 0;
}