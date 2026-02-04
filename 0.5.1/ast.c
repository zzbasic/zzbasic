// ast.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "a89alloc.h"
#include "color_mapping.h"

typedef struct
{
    const char* logic_operator_name;
    LogicalOperator operator;
} LogicOp;

static LogicOp logic_operators [] = 
{
    {"None", OP_NONE},
    {"AND", OP_AND},
    {"OR", OP_OR},
    {"NOT", OP_NOT},
    {"==", OP_EQUAL},
    {"!=", OP_NOT_EQUAL},
    {"<", OP_LESS},
    {">", OP_GREATER},
    {"<=", OP_LESS_EQUAL},
    {">=", OP_GREATER_EQUAL}
};

//===================================================================
// NODE CREATION FUNCTIONS
//===================================================================

// CREATES NODE AND SAFELY INITIALIZES ITS FIELDS WITH memset
static ASTNode* create_node(NodeType type, int line, int column)
{
    ASTNode* node = A89ALLOC(sizeof(ASTNode));
    node->type = type;
    node->line = line;
    node->column = column;
    memset(&node->data, 0, sizeof(node->data));
    return node;
}

ASTNode* create_bool_node(int value, int line, int column)
{
    ASTNode* node = create_node(NODE_BOOL, line, column);
    node->data.boolean.value = value;
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

ASTNode* create_input_node(const char* prompt, char* var_name, int line, int column)
{
    ASTNode* node = create_node(NODE_INPUT, line, column);
    if(prompt)
    {
        strncpy(node->data.inputstatement.prompt, prompt, STRING_SIZE - 1);
        node->data.inputstatement.prompt[STRING_SIZE - 1] = '\0';
    }
    strncpy(node->data.inputstatement.var_name, var_name, VARNAME_SIZE - 1);
    node->data.inputstatement.var_name[VARNAME_SIZE - 1] = '\0';
    return node;
}

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

// Criar nó de operação de comparação(==, !=, <, <=, >, >=)
ASTNode* create_comparison_op_node(LogicalOperator operator,
                                   ASTNode* left, ASTNode* right, 
                                   int line, int column) {
    ASTNode* node = create_node(NODE_COMPARISON_OP, line, column);
    node->line = line;
    node->column = column;
    
    node->data.logicalop.operator = operator;
    node->data.logicalop.left = left;
    node->data.logicalop.right = right;
    
    return node;
}

// Criar nó de operação lógica (AND, OR)
ASTNode* create_logical_op_node(LogicalOperator operator,
                                ASTNode* left, ASTNode* right, 
                                int line, int column) {
    ASTNode* node = create_node(NODE_LOGICAL_OP, line, column);
    node->line = line;
    node->column = column;
    
    node->data.logicalop.operator = operator;
    node->data.logicalop.left = left;
    node->data.logicalop.right = right;
    
    return node;
}

// Criar nó de operação lógica unária (NOT)
ASTNode* create_logical_not_node(ASTNode* operand, int line, int column)
{
    ASTNode* node = create_node(NODE_NOT_LOGICAL_OP, line, column);
    node->line = line;
    node->column = column;
    
    node->data.notop.operator = OP_NOT;
    node->data.notop.operand = operand;
    
    return node;
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

        case NODE_COMPARISON_OP:
            free_ast(node->data.logicalop.left);
            free_ast(node->data.logicalop.right);
            break;

        case NODE_LOGICAL_OP:
            free_ast(node->data.logicalop.left);
            free_ast(node->data.logicalop.right);
            break;

        case NODE_NOT_LOGICAL_OP:
            free_ast(node->data.notop.operand);
            break;            

        case NODE_BOOL:
        case NODE_NUMBER:
        case NODE_STRING:
        case NODE_VARIABLE:
        case NODE_INPUT:
        case NODE_COLOR:
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

    }
    
    a89free(node); 
}

void print_ast(ASTNode* node, int indent)
{
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("    ");
    
    printf("[%d:%d] ", node->line, node->column);
    
    switch (node->type)
    {
        case NODE_BOOL:
            printf("BOOL: %s\n", (node->data.boolean.value ? "true" : "false"));
            break;

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

        case NODE_INPUT:
            printf("INPUT: variable: %s; prompt: %s \n",
                   node->data.inputstatement.var_name, node->data.inputstatement.prompt);
            break;

        case NODE_PRINT:
            printf("PRINT (%d items)", node->data.printstatement.count);
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

        case NODE_COMPARISON_OP:
            printf("COMPARISON_OP: '%s'\n",
            logic_operators[node->data.logicalop.operator].logic_operator_name);
            print_ast(node->data.logicalop.left, indent + 1);
            print_ast(node->data.logicalop.right, indent + 1);
            break;

        case NODE_LOGICAL_OP:
            printf("LOGICAL_OP: '%s'\n",
            logic_operators[node->data.logicalop.operator].logic_operator_name);
            print_ast(node->data.logicalop.left, indent + 1);
            print_ast(node->data.logicalop.right, indent + 1);
            break;

        case NODE_NOT_LOGICAL_OP:
            printf("NOT_LOGICAL_OP: '%s'\n",
            logic_operators[node->data.notop.operator].logic_operator_name);
            print_ast(node->data.notop.operand, indent + 1);
            break;

    }
}


#ifdef TESTAST
#include "utils.h"
#include "color.h"
int main(void)
{
    setup_utf8();

    printf("ZzBasic AST Test v0.5.1 - operações de comparação e lógicas'\n\n");
   

    ASTNode* left1 = create_number_node(5, 1, 1);
    ASTNode* right1 = create_number_node(7, 1, 5);
    ASTNode* comparison_op1 = create_comparison_op_node(OP_EQUAL, left1, right1, 1, 3);
    printf("comparison_op1 node:\n");
    print_ast(comparison_op1, 0);
    printf("\n");

    free_ast(comparison_op1);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5);

    ASTNode* comparison_op2 = create_comparison_op_node(OP_NOT_EQUAL, left1, right1, 1, 3);
    printf("comparison_op2 node:\n");
    print_ast(comparison_op2, 0);
    printf("\n");

    free_ast(comparison_op2);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5);

    ASTNode* comparison_op3 = create_comparison_op_node(OP_LESS, left1, right1, 1, 3);
    printf("comparison_op3 node:\n");
    print_ast(comparison_op3, 0);
    printf("\n");    

    free_ast(comparison_op3);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5);

    ASTNode* comparison_op4 = create_comparison_op_node(OP_GREATER, left1, right1, 1, 3);
    printf("comparison_op4 node:\n");
    print_ast(comparison_op4, 0);
    printf("\n"); 

    free_ast(comparison_op4);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5);

    ASTNode* comparison_op5 = create_comparison_op_node(OP_LESS_EQUAL, left1, right1, 1, 3);
    printf("comparison_op5 node:\n");
    print_ast(comparison_op5, 0);
    printf("\n"); 

    free_ast(comparison_op5);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5);  

    ASTNode* comparison_op6 = create_comparison_op_node(OP_GREATER_EQUAL, left1, right1, 1, 3);
    printf("comparison_op6 node:\n");
    print_ast(comparison_op6, 0);
    printf("\n");

    free_ast(comparison_op6);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5); 

    ASTNode* comparison_op7 = create_logical_op_node(OP_AND, left1, right1, 1, 3);
    printf("comparison_op7 node:\n");
    print_ast(comparison_op7, 0);
    printf("\n");

    free_ast(comparison_op7);

    left1 = create_number_node(5, 1, 1);
    right1 = create_number_node(7, 1, 5); 

    ASTNode* comparison_op8 = create_logical_op_node(OP_OR, left1, right1, 1, 3);
    printf("comparison_op8 node:\n");
    print_ast(comparison_op8, 0);
    printf("\n");

    free_ast(comparison_op8);

    left1 = create_number_node(5, 1, 1);

    ASTNode* comparison_op9 = create_logical_not_node(left1, 1, 3);
    printf("comparison_op9 node:\n");
    print_ast(comparison_op9, 0);
    printf("\n");

    free_ast(comparison_op9);

    printf("Memória liberada. Teste concluído.\n");

    a89check_leaks();
    return 0;
}
#endif
// Fim de ast.c

