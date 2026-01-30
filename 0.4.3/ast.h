// ast.h

#ifndef AST_H
#define AST_H

#include "zzdefs.h"
#include "color_mapping.h"

typedef struct ASTNode ASTNode;

//===================================================================
// ENUMS
//===================================================================
typedef enum
{
    NODE_NULL,
    NODE_NUMBER,
    NODE_STRING,       
    NODE_VARIABLE,    
    NODE_BINARY_OP,     // operacao binaria (+, -, *, /)
    NODE_UNARY_OP,      // operacao unaria (+, -)
    NODE_ASSIGNMENT,    // atribuição  
    NODE_STATEMENT_LIST,
    NODE_PRINT,
    NODE_COLOR,
    NODE_ALIGNMENT,
    NODE_WIDTH, 
    NODE_INPUT 
} NodeType;

typedef enum
{
    TYPE_NULL,
    TYPE_NUMBER,
    TYPE_STRING,
    // Tipos futuros...
    // TYPE_BOOLEAN,
    // TYPE_ARRAY,
    // TYPE_FUNCTION,
} VariableType;


//===================================================================
// STRUCTS
//===================================================================
typedef struct
{
    double value;
} NumberData;

typedef struct
{
    char value[STRING_SIZE];
} StringData;


typedef struct
{
    char operator;
    ASTNode* left;
    ASTNode* right;
} BinaryOpData;

typedef struct
{
    char operator;
    ASTNode* operand;
} UnaryOpData;


typedef struct
{
    char var_name[VARNAME_SIZE];
} VariableData;


typedef struct
{
    char var_name[VARNAME_SIZE];
    ASTNode* value;  // ASTNode que contém a expressão a ser atribuída
} AssignmentData;


typedef struct
{
    ASTNode** statements;  // Array de ponteiros para statements
    int count;             // Quantidade de statements
    int capacity;          // Capacidade do array (para redimensionamento)
} StatementListData;


typedef struct
{
    ASTNode** items;           // Array de ASTNode* (expressões)
    int count;
    int capacity;
    int newline;              // 1 - newline; 0 - mesma linha  
} PrintStatementData;

typedef struct
{
    int color_token_id;
    char ansi_color[15];  // Ex: "\033[31m" para vermelho
} ColorNodeData;

typedef struct {
    TokenType alignment_type;
} AlignmentNodeData;

typedef struct
{
    int value;
} WidthNodeData;

typedef struct {
    char prompt[STRING_SIZE];// Prompt opcional (ex: "Digite: ")
    char var_name[VARNAME_SIZE];// Nome da variável 
} InputStatementNode;

typedef struct ASTNode
{
    NodeType type;
    int line;
    int column;
    
    union
    {
        NumberData          number;
        StringData          string;
        VariableData        variable;
        BinaryOpData        binaryop;
        UnaryOpData         unaryop;
        AssignmentData      assignment;
        StatementListData   statementlist;
        PrintStatementData  printstatement;
        ColorNodeData       color;
        AlignmentNodeData   alignment;
        WidthNodeData       width;
        InputStatementNode  inputstatement;

    } data;

} ASTNode;


//===================================================================
// FUNCTIONS
//===================================================================
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

ASTNode* create_input_node(const char* prompt, char* var_name, int line, int column);

// Funções para criar nó print
ASTNode* create_print_node(int line, int column);
ASTNode* create_color_node(TokenType color_token, int line, int column);
ASTNode* create_alignment_node(TokenType alignment_token, int line, int column);
ASTNode* create_width_node(int width_value, int line, int column);

void print_node_add_item(ASTNode* print_node, ASTNode* expr_node);
void print_set_newline(ASTNode* print_node, int has_newline);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif // AST_H
// Fim de st.h