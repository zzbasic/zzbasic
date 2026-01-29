// ast.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "a89alloc.h"
#include "color_mapping.h"

/********************************************************************
 
 FUNÇÕES DESTE ARQUIVO

ASTNode* create_number_node(double value, int line, int column);
ASTNode* create_string_node(const char* value, int line, int column);
// CRIA NÓ DE VARIÁVEL. VAR_NAME JÁ DEVE SER VÁLIDO (VALIDADO PELO PARSER)
ASTNode* create_variable_node(const char* var_name, int line, int column);
// CRIA OPERAÇÃO BINÁRIA. LEFT E RIGHT NÃO PODEM SER NULL
ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, 
                               int line, int column);
// CRIA OPERAÇÃO UNÁRIA. OPERAND NÃO PODE SER NULL
ASTNode* create_unary_op_node(char operator, ASTNode* operand, 
                              int line, int column);
// CRIA ATRIBUIÇÃO. VAR_NAME JÁ VALIDADO, VALUE NÃO PODE SER NULL
ASTNode* create_assignment_node(const char* var_name, ASTNode* value, 
                                int line, int column);
ASTNode* create_statement_list_node(int line, int column);
void statement_list_add(ASTNode* list_node, ASTNode* stmt);
// Funções para criar nó print
ASTNode* create_print_node(int line, int column);
ASTNode* create_color_node(TokenType color_token, int line, int column);
void print_node_add_item(ASTNode* print_node, ASTNode* expr_node);
void print_set_newline(ASTNode* print_node, int has_newline);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);
********************************************************************/

//===================================================================
// NODE CREATION FUNCTIONS
//===================================================================


// CREATES NODE AND SAFELY INITIALIZES ITS FIELDS WITH memset
static ASTNode* create_node(NodeType type, int line, int column)
{
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: insufficient memory to create node\n");
        exit(EXIT_FAILURE);
    }
    
    node->type = type;
    node->line = line;
    node->column = column;
    
    /****************************************************************
    memset(&node->data, 0, sizeof(node->data));

    Zeros the ENTIRE 'data' union - ESSENTIAL to avoid undefined behavior!
    After memset, all union fields will have safe values:
    
    For NODE_NUMBER:
      - data.number.value = 0.0 (zeroed double)
    
    For NODE_STRING:
      - data.string.value = "" (empty string, first byte = '\0')
    
    For NODE_BINARY_OP:
      - data.binaryop.operator = '\0' (null character)
      - data.binaryop.left = NULL (null pointer)
      - data.binaryop.right = NULL (null pointer)
    
    For NODE_UNARY_OP:
      - data.unaryop.operator = '\0' (null character)
      - data.unaryop.operand = NULL (null pointer)
    
    For NODE_ASSIGNMENT:
      - data.assignment.var_name = "" (empty string)
      - data.assignment.value = NULL (null pointer)
    
    For NODE_VARIABLE:
      - data.variable.var_name = "" (empty string)
    
    IMPORTANT: Since it's a union, only ONE of these field sets
    will be active at a time (defined by the node's 'type' field).
    memset ensures that, regardless of the type, there won't be:
    - Invalid pointers (all will be NULL)
    - Strings with garbage (all will be empty)
    - Random numeric values (all will be 0.0)
    - Random characters in operators (all will be '\0')
    
    WITHOUT this memset, the memory would contain random values ("garbage"),
    which could cause:
    - Segmentation faults when dereferencing invalid pointers
    - Undefined behavior when reading unterminated strings
    - Hard-to-diagnose bugs in operations
    - Incorrect values in numeric calculations
    
    This memset is MANDATORY for program correctness and safety.
    memset is in <string.h>
    ****************************************************************/
    memset(&node->data, 0, sizeof(node->data));
    
    return node;
}

ASTNode* create_number_node(double value, int line, int column)
{
    ASTNode* node = create_node(NODE_NUMBER, line, column);
    node->data.number.value = value;
    return node;
}

ASTNode* create_string_node(const char* value, int line, int column)
{
    ASTNode* node = create_node(NODE_STRING, line, column);
    
    if (value)
    {
        strncpy(node->data.string.value, value, STRING_SIZE - 1);
        node->data.string.value[STRING_SIZE - 1] = '\0';
    }
    else
    {
        node->data.string.value[0] = '\0';
    }
    
    return node;
}

// CREATES VARIABLE NODE. VAR_NAME MUST ALREADY BE VALID (VALIDATED BY PARSER)
ASTNode* create_variable_node(const char* var_name, int line, int column)
{
    ASTNode* node = create_node(NODE_VARIABLE, line, column);
    strncpy(node->data.variable.var_name, var_name, VARNAME_SIZE - 1);
    node->data.variable.var_name[VARNAME_SIZE - 1] = '\0';
    return node;
}

// CREATES BINARY OPERATION. LEFT AND RIGHT CANNOT BE NULL
ASTNode* create_binary_op_node(char operator, ASTNode* left, ASTNode* right, 
                               int line, int column)
{
    ASTNode* node = create_node(NODE_BINARY_OP, line, column);
    node->data.binaryop.operator = operator;
    node->data.binaryop.left = left;
    node->data.binaryop.right = right;
    return node;
}

// CREATES UNARY OPERATION. OPERAND CANNOT BE NULL
ASTNode* create_unary_op_node(char operator, ASTNode* operand, 
                              int line, int column)
{
    ASTNode* node = create_node(NODE_UNARY_OP, line, column);
    node->data.unaryop.operator = operator;
    node->data.unaryop.operand = operand;
    return node;
}

// CREATES ASSIGNMENT. VAR_NAME ALREADY VALIDATED, VALUE CANNOT BE NULL
ASTNode* create_assignment_node(const char* var_name, ASTNode* value, 
                                int line, int column)
{
    ASTNode* node = create_node(NODE_ASSIGNMENT, line, column);
    strncpy(node->data.assignment.var_name, var_name, VARNAME_SIZE - 1);
    node->data.assignment.var_name[VARNAME_SIZE - 1] = '\0';
    node->data.assignment.value = value;
    return node;
}

// Cria um nó de lista de statements
ASTNode* create_statement_list_node(int line, int column) {
    ASTNode* node = create_node(NODE_STATEMENT_LIST, line, column);
    node->data.statementlist.capacity = 4;
    node->data.statementlist.statements = A89ALLOC(
        sizeof(ASTNode*) * node->data.statementlist.capacity
    );
    return node;
}

// Adiciona um statement à lista (redimensiona se necessário)
void statement_list_add(ASTNode* list_node, ASTNode* stmt)
{
    if (list_node->type != NODE_STATEMENT_LIST)
    {
        fprintf(stderr, "Error: not a statement list node\n");
        return;
    }
    
    StatementListData* list = &list_node->data.statementlist;
    
    // Redimensiona se necessário
    if (list->count >= list->capacity)
    {
        int new_cap = list->capacity * 2;
        
        ASTNode** new_exprs = A89ALLOC(new_cap * sizeof(ASTNode*));
        
        for (int i = 0; i < list->capacity; i++) {
            new_exprs[i] = list->statements[i];
        }
        
        a89free(list->statements);
        
        list->statements = new_exprs;
        list->capacity = new_cap;
    }
    
    list->statements[list->count] = stmt;
    list->count++;
}


// Cria um nó de comando print
ASTNode* create_print_node(int line, int column)
{
    ASTNode* node = create_node(NODE_PRINT, line, column);
    
    // Inicializa a estrutura printstatement
    node->data.printstatement.capacity = 4;  // Começa com capacidade para 4 itens
    node->data.printstatement.items = A89ALLOC(
        sizeof(ASTNode*) * node->data.printstatement.capacity
    );
    node->data.printstatement.count = 0;
    node->data.printstatement.newline = 0;
    
    return node;
}


ASTNode* create_color_node(TokenType color_token, int line, int column)
{
    ASTNode* node = create_node(NODE_COLOR, line, column);
    
    // 1. Converte TokenType para ColorCode
    ColorCode color_code = token_to_color_code(color_token);
    node->data.color.color_token_id = color_code;
    
    // 2. Obtém string ANSI
    const char* ansi = token_to_ansi(color_token);
    
    // 3. Copia para o node
    if (ansi != NULL) {
        strncpy(node->data.color.ansi_color, ansi, 
                sizeof(node->data.color.ansi_color) - 1);
        node->data.color.ansi_color[sizeof(node->data.color.ansi_color) - 1] = '\0';
    } else {
        strcpy(node->data.color.ansi_color, "\033[0m");
    }
    
    return node;
}

ASTNode* create_alignment_node(TokenType token_type, int line, int column)
{
    ASTNode* node = create_node(NODE_ALIGNMENT, line, column);
    node->data.alignment.alignment_type = token_type;
    return node;
}

ASTNode* create_width_node(int width_value, int line, int column)
{
    ASTNode* node = create_node(NODE_WIDTH, line, column);
    node->data.width.value = width_value;
    return node;
}

// Adiciona um item (expressão) ao comando print
void print_node_add_item(ASTNode* print_node, ASTNode* item)
{
    if (print_node->type != NODE_PRINT)
    {
        fprintf(stderr, "Error: not a print statement node\n");
        return;
    }
    
    PrintStatementData* print_data = &print_node->data.printstatement;
    
    // Redimensiona se necessário
    if (print_data->count >= print_data->capacity)
    {
        int new_cap = print_data->capacity * 2;
        ASTNode** new_items = A89ALLOC(new_cap * sizeof(ASTNode*));
        
        // Copia os itens antigos
        for (int i = 0; i < print_data->capacity; i++) {
            new_items[i] = print_data->items[i];
        }
        
        // Libera o array antigo
        a89free(print_data->items);
        
        // Atualiza para o novo array
        print_data->items = new_items;
        print_data->capacity = new_cap;
    }
    
    // Adiciona o novo item
    print_data->items[print_data->count] = item;
    print_data->count++;
}

void print_set_newline(ASTNode* print_node, int has_newline) {
    if (print_node->type != NODE_PRINT) return;
    print_node->data.printstatement.newline = has_newline;
}
    
//===================================================================
// MEMORY DEALLOCATION
//===================================================================
void free_ast(ASTNode* node)
{
    if (!node) return;
    
    switch (node->type) {
        case NODE_BINARY_OP:
            free_ast(node->data.binaryop.left);
            free_ast(node->data.binaryop.right);
            break;
            
        case NODE_UNARY_OP:
            free_ast(node->data.unaryop.operand);
            break;
            
        case NODE_ASSIGNMENT:
            free_ast(node->data.assignment.value);
            break;
            
        case NODE_STATEMENT_LIST:
        {
            // Libera cada statement individual
            for (int i = 0; i < node->data.statementlist.count; i++) {
                free_ast(node->data.statementlist.statements[i]);
            }
            
            // Libera o array de ponteiros
            if (node->data.statementlist.statements != NULL) {
                a89free(node->data.statementlist.statements);
            }
            
            break;
        }
            
        case NODE_NUMBER:
        case NODE_STRING:
        case NODE_VARIABLE:
        case NODE_NULL:
            // No children to free
            break;

        case NODE_PRINT:
        {
            // Libera cada item (expressão) individualmente
            for (int i = 0; i < node->data.printstatement.count; i++)
            {
                free_ast(node->data.printstatement.items[i]);
            }
            
            // Libera o array de ponteiros
            if (node->data.printstatement.items != NULL)
            {
                a89free(node->data.printstatement.items);
            }
            break;
        }

        case NODE_COLOR:
            // ColorNodeData não tem filhos para liberar
            break;

    }
    
    a89free(node); 
}

void print_ast(ASTNode* node, int indent)
{
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("    ");
    
    printf("[%d:%d] ", node->line, node->column);
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %g\n", node->data.number.value);
            break;
            
        case NODE_BINARY_OP:
            printf("BINARY_OP: '%c'\n", node->data.binaryop.operator);
            print_ast(node->data.binaryop.left, indent + 1);
            print_ast(node->data.binaryop.right, indent + 1);
            break;
            
        case NODE_UNARY_OP:
            printf("UNARY_OP: '%c'\n", node->data.unaryop.operator);
            print_ast(node->data.unaryop.operand, indent + 1);
            break;
            
        case NODE_VARIABLE:
            printf("VARIABLE: %s\n", node->data.variable.var_name);
            break;
            
        case NODE_ASSIGNMENT:
            printf("ASSIGNMENT: %s\n", node->data.assignment.var_name);
            print_ast(node->data.assignment.value, indent + 1);
            break;

        case NODE_STATEMENT_LIST:  
            printf("STATEMENT_LIST (%d statements)\n", 
                   node->data.statementlist.count);
            for (int i = 0; i < node->data.statementlist.count; i++) {
                print_ast(node->data.statementlist.statements[i], indent + 1);
            }
            break;
            
        case NODE_STRING:
            printf("STRING: \"%s\"\n", node->data.string.value);
            break;
            
        case NODE_NULL:
            printf("NULL\n");
            break;

        case NODE_PRINT:
            printf("PRINT_STATEMENT (%d items)", node->data.printstatement.count);
            if (node->data.printstatement.newline)
            {
                printf(" [newline]");
            }
            printf("\n");
            for (int i = 0; i < node->data.printstatement.count; i++)
            {
                print_ast(node->data.printstatement.items[i], indent + 1);
            }
            break;

        case NODE_COLOR:
            printf("COLOR: token_id=%d\n", 
                   node->data.color.color_token_id);
            break;

        case NODE_ALIGNMENT:
            printf("ALIGNMENT: "); 
            switch(node->data.alignment.alignment_type)
            {
                case TOKEN_LEFT:
                    printf("LEFT\n");break;
                case TOKEN_RIGHT:
                    printf("RIGHT\n");break;
                case TOKEN_CENTER:
                    printf("CENTER\n");break;
            }
            break;

        case NODE_WIDTH:
            printf("WIDTH: [%d]\n", node->data.width.value); 
            break;
    }
}

#ifdef TESTAST
#include "utils.h"
#include "color.h"
int main(void)
{
    setup_utf8();
    
    printf("%s=== TESTE CRIAÇÃO DE NODES ===%s\n\n", COLOR_HEADER, COLOR_RESET);
/*    
    // 1. Number
    ASTNode* num = create_number_node(42.5, 1, 1);
    printf("%s1. Número:%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(num, 0);
    printf("\n");
    wait();

    // 2. String
    ASTNode* str = create_string_node("Olá Mundo", 2, 1);
    printf("%s2. String:%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(str, 0);
    printf("\n");
    wait();
    
    // 3. Variable
    ASTNode* var = create_variable_node("idade", 3, 1);
    printf("%s3. Variável:%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(var, 0);
    printf("\n");
    wait();

    // 4. Binary operation: 10 + 20
    ASTNode* num1 = create_number_node(10.0, 4, 1);
    ASTNode* num2 = create_number_node(20.0, 4, 5);
    ASTNode* soma = create_binary_op_node('+', num1, num2, 4, 3);
    printf("%s4. Operação binária (10 + 20):%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(soma, 0);
    printf("\n");
    wait();

    // 5. Unary operation: -15
    ASTNode* num3 = create_number_node(15.0, 5, 2);
    ASTNode* negativo = create_unary_op_node('-', num3, 5, 1);
    printf("%s5. Operação unária (-15):%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(negativo, 0);
    printf("\n");
    wait();

    // 6. Assignment: x = 100
    ASTNode* num4 = create_number_node(100.0, 6, 5);
    ASTNode* atrib = create_assignment_node("x", num4, 6, 1);
    printf("%s6. Atribuição (x = 100):%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(atrib, 0);
    printf("\n");
    wait();

    // 7. Complex expression: y = (5 + 3) * 2
    ASTNode* num5 = create_number_node(5.0, 7, 7);
    ASTNode* num6 = create_number_node(3.0, 7, 11);
    ASTNode* num7 = create_number_node(2.0, 7, 15);
    ASTNode* soma2 = create_binary_op_node('+', num5, num6, 7, 9);
    ASTNode* mult = create_binary_op_node('*', soma2, num7, 7, 13);
    ASTNode* atrib2 = create_assignment_node("y", mult, 7, 1);

    printf("%s7. Expressão complexa (y = (5 + 3) * 2):%s\n", COLOR_HEADER, COLOR_RESET);
    print_ast(atrib2, 0);
    wait();

    // ============================================
    // TESTE 8: Comando PRINT
    // ============================================
    printf("\n%s8. Comando PRINT:%s\n", COLOR_HEADER, COLOR_RESET);
    
    // 8.1 Print simples com string
    printf("8.1 Print simples com string:\n");
    ASTNode* print1 = create_print_node(8, 1);
    ASTNode* str1 = create_string_node("Olá Mundo", 8, 7);
    print_node_add_item(print1, str1);
    print_ast(print1, 0);
    printf("\n");
    wait();

    // 8.2 Print com múltiplos itens
    printf("8.2 Print com múltiplos itens:\n");
    ASTNode* print2 = create_print_node(9, 1);
    ASTNode* str2a = create_string_node("Nome:", 9, 7);
    ASTNode* var2 = create_variable_node("usuario", 9, 14);
    ASTNode* str2b = create_string_node("Idade:", 9, 22);
    num2 = create_number_node(25, 9, 29);

    print_node_add_item(print2, str2a);
    print_node_add_item(print2, var2);
    print_node_add_item(print2, str2b);
    print_node_add_item(print2, num2);
    print_ast(print2, 0);
    printf("\n");
    wait();

    // 8.3 Print com expressão matemática
    printf("8.3 Print com expressão matemática:\n");
    ASTNode* print3 = create_print_node(10, 1);
    ASTNode* str3a = create_string_node("Soma:", 10, 7);
    ASTNode* num3a = create_number_node(10, 10, 14);
    ASTNode* num3b = create_number_node(20, 10, 19);
    ASTNode* expr3 = create_binary_op_node('+', num3a, num3b, 10, 17);
    ASTNode* str3b = create_string_node("Resultado:", 10, 23);
    
    print_node_add_item(print3, str3a);
    print_node_add_item(print3, expr3);
    print_node_add_item(print3, str3b);
    print_ast(print3, 0);
    printf("\n");
    wait();

    // 8.4 Print com set_newline (nl no final)
    printf("8.4 Print com set_newline (nl no final):\n");
    ASTNode* print4 = create_print_node(11, 1);
    ASTNode* str4 = create_string_node("Linha 1 nl", 11, 7);
    print_node_add_item(print4, str4);
    print_set_newline(print4, 1);
    print_ast(print4, 0);
    printf("\n");
    wait();

    // 8.5 Print complexo: variável com expressão
    printf("8.5 Print complexo (variável com expressão):\n");
    ASTNode* print5 = create_print_node(12, 1);
    ASTNode* var5a = create_variable_node("preco", 12, 7);
    ASTNode* str5 = create_string_node("*", 12, 13);
    ASTNode* var5b = create_variable_node("quantidade", 12, 15);
    ASTNode* str5b = create_string_node("=", 12, 26);
    ASTNode* num5a = create_number_node(10.5, 12, 28);
    ASTNode* num5b = create_number_node(3, 12, 33);
    ASTNode* expr5 = create_binary_op_node('*', num5a, num5b, 12, 31);
    
    print_node_add_item(print5, var5a);
    print_node_add_item(print5, str5);
    print_node_add_item(print5, var5b);
    print_node_add_item(print5, str5b);
    print_node_add_item(print5, expr5);
    print_ast(print5, 0);
    wait();
*/

    // 8.6 Print com width e alignment
    printf("8.6 Print com width e alignment:\n");
    ASTNode* print6 = create_print_node(9, 1);
    ASTNode* item61 = create_width_node(10, 9, 7);
    TokenType token_type = TOKEN_LEFT;
    ASTNode* item62 = create_alignment_node(token_type, 9, 17);
    ASTNode* item63 = create_string_node("Nome:", 9, 19);
    ASTNode* item64 = create_width_node(20, 9, 7);
    ASTNode* item65 = create_variable_node("usuario", 9, 14);

    print_node_add_item(print6, item61);
    print_node_add_item(print6, item62);
    print_node_add_item(print6, item63);
    print_node_add_item(print6, item64);
    print_node_add_item(print6, item65);
    print_ast(print6, 0);
    printf("\n");
    wait();

    // Free everything
    printf("\n=== LIMPEZA ===\n");
    // free_ast(num);
    // free_ast(str);
    // free_ast(var);
    // free_ast(soma);      // Frees num1 and num2 too
    // free_ast(negativo);  // Frees num3 too
    // free_ast(atrib);     // Frees num4 too
    // free_ast(atrib2);    // Frees entire complex tree
    // free_ast(print1);
    // free_ast(print2);
    // free_ast(print3);
    // free_ast(print4);
    // free_ast(print5);
    free_ast(print6);
    
    printf("Memória liberada. Teste concluído.\n");

    a89check_leaks();
    return 0;
}
#endif
// END OF ast.c