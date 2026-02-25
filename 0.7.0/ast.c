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

ASTNode* create_assignment_node(ASTNode* target, ASTNode* value, 
                                int line, int column)
{
    if (!target || !value)
    {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_ASSIGNMENT, line, column);
    node->data.assignment.target = target;
    node->data.assignment.value = value;
    return node;
}

ASTNode* create_let_node(ASTNode* target, ASTNode* value, 
                         int line, int column)
{
    if (!target || !value)
    {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_LET, line, column);
    node->data.let_stmt.target = target;
    node->data.let_stmt.value = value;
    return node;
}

// Cria um nó de lista de statements
ASTNode* create_statement_list_node(int line, int column) {
    ASTNode* node = create_node(NODE_STATEMENT_LIST, line, column);
    node->data.stmt_list.capacity = 4;
    node->data.stmt_list.statements = A89ALLOC(
        sizeof(ASTNode*) * node->data.stmt_list.capacity
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
    
    StatementListData* list = &list_node->data.stmt_list;
    
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

ASTNode* create_input_node(ASTNode* color, ASTNode* width, ASTNode* alignment,
                          const char* prompt,
                          int set_nocolor,
                          char* var_name,
                          int line, int column)
{
    ASTNode* node = create_node(NODE_INPUT, line, column);
    node->data.input_stmt.color = color;
    node->data.input_stmt.width = width;
    node->data.input_stmt.alignment = alignment;
    node->data.input_stmt.set_nocolor = set_nocolor;
    if(prompt)
    {
        strncpy(node->data.input_stmt.prompt, prompt, STRING_SIZE - 1);
        node->data.input_stmt.prompt[STRING_SIZE - 1] = '\0';
    }
    strncpy(node->data.input_stmt.var_name, var_name, VARNAME_SIZE - 1);
    node->data.input_stmt.var_name[VARNAME_SIZE - 1] = '\0';
  
    return node;
}


ASTNode* create_print_node(int line, int column)
{
    ASTNode* node = create_node(NODE_PRINT, line, column);
    
    // Inicializa a estrutura print_stmt
    node->data.print_stmt.capacity = 4;  // Começa com capacidade para 4 itens
    node->data.print_stmt.items = A89ALLOC(
        sizeof(ASTNode*) * node->data.print_stmt.capacity
    );
    node->data.print_stmt.count = 0;
    node->data.print_stmt.newline = 0;
    
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
    
    PrintStatementData* print_data = &print_node->data.print_stmt;
    
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
    print_node->data.print_stmt.newline = has_newline;
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

ASTNode* create_if_node(ASTNode* condition,
                        ASTNode* then_body, ASTNode* else_body,
                        int line, int column)
{
    if (!condition || !then_body)
    {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_IF, line, column);
    node->line = line;
    node->column = column;

    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_body = then_body;
    node->data.if_stmt.else_body = else_body;
    
    return node;
}

ASTNode* create_while_node(ASTNode* condition, ASTNode* body, int line, int column)
{
    ASTNode* node = create_node(NODE_WHILE, line, column);
    node->line = line;
    node->column = column;

    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    
    return node;
}

ASTNode* create_break_node(int line, int column)
{
    ASTNode* node = create_node(NODE_BREAK, line, column);
    node->line = line;
    node->column = column;
    
    return node;
}

ASTNode* create_continue_node(int line, int column)
{
    ASTNode* node = create_node(NODE_CONTINUE, line, column);
    node->line = line;
    node->column = column;
    
    return node;
}

ASTNode* create_for_node(char var_name[],
                         ASTNode* init_value, 
                         ASTNode* end_value,
                         ASTNode* step_value, 
                         ASTNode* body,
                         int line, int column)
{
    ASTNode* node = create_node(NODE_FOR, line, column);
    node->line = 0;
    node->column = 0;
    
    strncpy(node->data.for_stmt.var_name, var_name, VARNAME_SIZE - 1);
    node->data.for_stmt.var_name[VARNAME_SIZE - 1] = '\0';

    node->data.for_stmt.init_value = init_value;
    node->data.for_stmt.end_value = end_value;
    node->data.for_stmt.step_value = step_value;
    node->data.for_stmt.body = body;

    return node;
}

ASTNode* create_import_node(const char* module_name,
                            char** imported_names,
                            int imported_count,
                            int import_all,
                            int line, int column)
{
    ASTNode* node = create_node(NODE_IMPORT, line, column);
    node->line = line;
    node->column = column;
    
    strncpy(node->data.import_stmt.module_name, module_name, VARNAME_SIZE - 1);
    node->data.import_stmt.module_name[VARNAME_SIZE - 1] = '\0';
    
    node->data.import_stmt.imported_names = imported_names;
    node->data.import_stmt.imported_count = imported_count;
    node->data.import_stmt.import_all = import_all;
    
    return node;
}

ASTNode* create_load_node(ASTNode* filename_expr, int line, int column)
{
    ASTNode* node = create_node(NODE_LOAD, line, column);
    node->line = line;
    node->column = column;
    node->data.load_expr.filename_expr = filename_expr;
    return node;
}

ASTNode* create_save_node(ASTNode* expression, const char* filename, int line, int column)
{
    ASTNode* node = create_node(NODE_SAVE, line, column);
    node->line = line;
    node->column = column;
    node->data.save_stmt.expression = expression;
    strncpy(node->data.save_stmt.filename, filename, BUFFER_SIZE - 1);
    node->data.save_stmt.filename[BUFFER_SIZE - 1] = '\0';
    return node;
}

ASTNode* create_function_call_node(const char* function_name, int line, int column)
{
    ASTNode* node = create_node(NODE_FUNCTION_CALL, line, column);
    
    strncpy(node->data.function_call.function_name, function_name, BUFFER_SIZE - 1);
    node->data.function_call.function_name[BUFFER_SIZE - 1] = '\0';
    node->data.function_call.arguments = NULL;
    node->data.function_call.arg_count = 0;
    
    return node;
}

void function_call_add_argument(ASTNode* node, ASTNode* argument)
{
    if (!node || node->type != NODE_FUNCTION_CALL || !argument) return;
    
    FunctionCallData* function = &node->data.function_call;
    
    // Realoca array de argumentos
    ASTNode** new_args = A89REALLOC(function->arguments, 
                                   (function->arg_count + 1) * sizeof(ASTNode*));
    if (!new_args) return;
    
    function->arguments = new_args;
    function->arguments[function->arg_count] = argument;
    function->arg_count++;
}

ASTNode* create_array_node(int line, int column)
{
    ASTNode* node = create_node(NODE_ARRAY, line, column);
    return node;
}

ASTNode* create_array_access_node(ASTNode* array, ASTNode* index, int line, int column)
{
    ASTNode* node = create_node(NODE_ARRAY_INDEX, line, column);
    node->data.array_index.array = array;
    node->data.array_index.index = index;
    return node;
}


//===================================================================
// LIBERACAO DA MEMORIA
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
            free_ast(node->data.assignment.target);  
            free_ast(node->data.assignment.value);
            break;
            
        case NODE_STATEMENT_LIST:
        {
            // Libera cada statement individual
            for (int i = 0; i < node->data.stmt_list.count; i++) {
                free_ast(node->data.stmt_list.statements[i]);
            }
            
            // Libera o array de ponteiros
            if (node->data.stmt_list.statements != NULL) {
                a89free(node->data.stmt_list.statements);
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

        case NODE_PRINT:
        {
            // Libera cada item (expressão) individualmente
            for (int i = 0; i < node->data.print_stmt.count; i++)
            {
                free_ast(node->data.print_stmt.items[i]);
            }
            
            // Libera o array de ponteiros
            if (node->data.print_stmt.items != NULL)
            {
                a89free(node->data.print_stmt.items);
            }
            break;
        }

        case NODE_INPUT:
            if(node->data.input_stmt.color)
            {
                free_ast(node->data.input_stmt.color);
            }
            if(node->data.input_stmt.width)
            {
                free_ast(node->data.input_stmt.width);
            }
            if(node->data.input_stmt.alignment)
            {
                free_ast(node->data.input_stmt.alignment);
            }
            break;        

        case NODE_IF:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_body);
            if (node->data.if_stmt.else_body) {
                free_ast(node->data.if_stmt.else_body);
            }
            break;

        case NODE_WHILE:
            if (node->data.while_stmt.condition)
            {
                free_ast(node->data.while_stmt.condition);
            } 
            if (node->data.while_stmt.body)
            {
                free_ast(node->data.while_stmt.body);
            } 
            break;

        case NODE_FOR:
            if (node->data.for_stmt.init_value)
            {
                free_ast(node->data.for_stmt.init_value);
            }
            if (node->data.for_stmt.end_value)
            {
                free_ast(node->data.for_stmt.end_value);
            }
            if (node->data.for_stmt.step_value)
            {
                free_ast(node->data.for_stmt.step_value);
            }
            if (node->data.for_stmt.body)
            {
                free_ast(node->data.for_stmt.body);
            }
            break;

        case NODE_IMPORT:
            if (node->data.import_stmt.imported_names)
            {
                for (int i = 0; i < node->data.import_stmt.imported_count; i++)
                {
                    if (node->data.import_stmt.imported_names[i])
                    {
                        a89free(node->data.import_stmt.imported_names[i]);
                    }
                }
                a89free(node->data.import_stmt.imported_names);
            }
            break;

        case NODE_LOAD:
            free_ast(node->data.load_expr.filename_expr);
            break;
                
        case NODE_SAVE:
            if (node->data.save_stmt.expression)
            {
                free_ast(node->data.save_stmt.expression);
            }
            break;

        case NODE_FUNCTION_CALL:
        {
            // Libera cada argumento
            for (int i = 0; i < node->data.function_call.arg_count; i++)
            {
                free_ast(node->data.function_call.arguments[i]);
            }
            
            // Libera array de argumentos
            if (node->data.function_call.arguments)
            {
                a89free(node->data.function_call.arguments);
            }
            break;
        }

        case NODE_ARRAY:
            // Array é gerenciado pela symbol table
            // Aqui não precisa fazer nada
            break;

        case NODE_ARRAY_INDEX:
            if(node->data.array_index.array)
            {
                free_ast(node->data.array_index.array);
            }
            if(node->data.array_index.index)
            {
                free_ast(node->data.array_index.index);
            }
            break;

        case NODE_LET:
            free_ast(node->data.let_stmt.target);
            free_ast(node->data.let_stmt.value);
            break;

        case NODE_BOOL:
        case NODE_NUMBER:
        case NODE_STRING:
        case NODE_VARIABLE:
        case NODE_COLOR:
        case NODE_BREAK:
        case NODE_CONTINUE:
        case NODE_NULL:
            // No children to free
            break;

    }
    
    a89free(node); 
} // Fim de free_ast()

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
            printf("ASSIGNMENT:\n");
            printf("%*sTarget:\n", (indent + 1) * 2, "");
            print_ast(node->data.assignment.target, indent + 2);
            printf("%*sValue:\n", (indent + 1) * 2, "");
            print_ast(node->data.assignment.value, indent + 2);
            break;

        case NODE_STATEMENT_LIST:  
            printf("STATEMENT_LIST (%d statements)\n", 
                   node->data.stmt_list.count);
            for (int i = 0; i < node->data.stmt_list.count; i++) {
                print_ast(node->data.stmt_list.statements[i], indent + 1);
            }
            break;
            
        case NODE_STRING:
            printf("STRING: \"%s\"\n", node->data.string.value);
            break;
            
        case NODE_NULL:
            printf("NULL\n");
            break;

        case NODE_INPUT:
            printf("INPUT:\n");
            printf("Variable: %s\n", node->data.input_stmt.var_name);
            printf("Prompt: %s\n", node->data.input_stmt.prompt);
            printf("Color:\n");
            if(node->data.input_stmt.color){
                print_ast(node->data.input_stmt.color, indent + 1);
            }
            printf("Width:\n");
            if(node->data.input_stmt.width){
                print_ast(node->data.input_stmt.width, indent + 1);
            }  
            printf("Alignment:\n");
            if(node->data.input_stmt.alignment){
                print_ast(node->data.input_stmt.alignment, indent + 1);
            } 
            break;

        case NODE_PRINT:
            printf("PRINT (%d items)", node->data.print_stmt.count);
            if (node->data.print_stmt.newline)
            {
                printf(" [newline]");
            }
            printf("\n");
            for (int i = 0; i < node->data.print_stmt.count; i++)
            {
                print_ast(node->data.print_stmt.items[i], indent + 1);
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

        case NODE_IF:
            printf("NODE IF\n");
            printf("Condition:\n");
            print_ast(node->data.if_stmt.condition, indent + 1);
            printf("Then block:\n");
            print_ast(node->data.if_stmt.then_body, indent + 1);
            if (node->data.if_stmt.else_body) {
                printf("Else block:\n");
                print_ast(node->data.if_stmt.else_body, indent + 1);
            }
            break;

        case NODE_WHILE:
            printf("NODE WHILE\n");
            printf("Condition:\n");
            print_ast(node->data.while_stmt.condition, indent + 1);
            printf("Body:\n");
            print_ast(node->data.while_stmt.body, indent + 1);
            break;

        case NODE_BREAK:
            printf("NODE BREAK\n");
            break;

        case NODE_CONTINUE:
            printf("NODE CONTINUE\n");
            break;

        case NODE_FOR:
            printf("NODE FOR\n");
            printf("Variable: %s\n", node->data.for_stmt.var_name);
            printf("Init value:\n");
            print_ast(node->data.for_stmt.init_value, indent + 1);
            printf("End value:\n");
            print_ast(node->data.for_stmt.end_value, indent + 1);
            printf("Step value:\n");
            if(node->data.for_stmt.step_value)
            {
                print_ast(node->data.for_stmt.step_value, indent + 1);
            }
            printf("Body:\n");
            print_ast(node->data.for_stmt.body, indent + 1);
            break;

        case NODE_IMPORT:
            if(node->data.import_stmt.import_all)
            {
                printf("IMPORT %s\n", node->data.import_stmt.module_name);
            }
            else
            {
                printf("FROM %s IMPORT: ", node->data.import_stmt.module_name);
                for (int i = 0; i < node->data.import_stmt.imported_count; i++)
                {
                    printf("%s, ", node->data.import_stmt.imported_names[i]);
                }
                printf("\n");
            }
            break;

        case NODE_LOAD:
            printf("LOAD:\n");
            print_ast(node->data.load_expr.filename_expr, indent + 1);
            break;

            
        case NODE_SAVE:
            printf("SAVE to \"%s\"\n", node->data.save_stmt.filename);
            printf("Expression:\n");
            print_ast(node->data.save_stmt.expression, indent + 1);
            break;

        case NODE_FUNCTION_CALL:
        {
            printf("FUNCTION_CALL: %s(", node->data.function_call.function_name);
            for (int i = 0; i < node->data.function_call.arg_count; i++)
            {
                if (i > 0) printf(", ");
                printf("arg%d", i);
            }
            printf(")\n");
            for (int i = 0; i < node->data.function_call.arg_count; i++)
            {
                print_ast(node->data.function_call.arguments[i], indent + 1);
            }
            break;
        }

        case NODE_ARRAY:
            printf("ARRAY\n");
            break;


    }
}
// Fim de ast.c
