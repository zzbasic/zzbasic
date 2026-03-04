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
    NODE_BOOL,
    NODE_STRING,       
    NODE_VARIABLE,    
    NODE_BINARY_OP,         // operacao binaria (+, -, *, /)
    NODE_UNARY_OP,          // operacao unaria (+, -)
    NODE_LET,
    NODE_ASSIGNMENT,        // atribuição  
    NODE_STATEMENT_LIST,
    NODE_PRINT,
    NODE_COLOR,
    NODE_ALIGNMENT,
    NODE_WIDTH, 
    NODE_INPUT,
    NODE_COMPARISON_OP,     // Operação de comparação (==, !=, <, >, <=, >=)
    NODE_LOGICAL_OP,        // Operação lógica (AND, OR)
    NODE_NOT_LOGICAL_OP,    // Operação lógica unária (NOT,!) 
    NODE_IF,
    NODE_WHILE,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_FOR,
    NODE_IMPORT,
    NODE_LOAD,
    NODE_SAVE,
    NODE_FUNCTION_CALL,
    NODE_ARRAY,
    NODE_ARRAY_INDEX,
    NODE_EMPTY

} NodeType;

typedef enum
{
    OP_NONE,
    OP_AND,           // AND
    OP_OR,            // OR
    OP_NOT,           // NOT, !
    OP_EQUAL,         // ==
    OP_NOT_EQUAL,     // !=
    OP_LESS,          // <
    OP_GREATER,       // >
    OP_LESS_EQUAL,    // <=
    OP_GREATER_EQUAL  // >=
} LogicalOperator;

//===================================================================
// STRUCTS
//===================================================================
typedef struct
{
    enum
    {
        FALSE,
        TRUE
    } value;
} BoolData;

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
    char var_name[VARNAME_SIZE];
} VariableData;

typedef struct
{
    char operator;
    ASTNode* left;
    ASTNode* right;
} BinaryOpData;

typedef struct
{
    LogicalOperator operator;
    ASTNode* left;
    ASTNode* right;
} LogicalOpData;

typedef struct
{
    char operator;
    ASTNode* operand;
} UnaryOpData;

typedef struct
{
    LogicalOperator operator;
    ASTNode* operand;
} NotOpData;

typedef struct
{
    ASTNode* target; // NODE_VARIABLE ou NODE_ARRAY_INDEX  
    ASTNode* value;  // ASTNode que contém a expressão a ser atribuída
} LetStatementData;

typedef struct
{
    ASTNode* target; // NODE_VARIABLE ou NODE_ARRAY_INDEX
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


// input_stmt := 'input' (color_directive)? (width_spec)? (alignment)? (STRING)? IDENTIFIER
typedef struct
{
    ASTNode* color; 
    ASTNode* width; 
    ASTNode* alignment; 
    char prompt[STRING_SIZE];// Prompt opcional (ex: "Digite: ")
    int set_nocolor; // 1 - nocolor ativado; 0 - nocolor desativado
    char var_name[VARNAME_SIZE];// Nome da variável 
} InputStatementNode;

typedef struct
{
    ASTNode* condition;         // Expressão da condição
    ASTNode* then_body;         // Statements do then
    ASTNode* else_body;         // Statements do else (pode ser NULL)
} IfStatementData;

typedef struct
{
    ASTNode* condition;
    ASTNode* body;
} WhileStatementData;

typedef struct
{
    int dummy;  // break não precisa de dados
} BreakStatementData;

typedef struct
{
    int dummy;  // continue não precisa de dados
} ContinueStatementData;

typedef struct
{
    char var_name[VARNAME_SIZE];
    ASTNode* init_value;    
    ASTNode* end_value;    
    ASTNode* step_value;   
    ASTNode* body;        
} ForStatementData;

typedef struct
{
    char module_name[VARNAME_SIZE]; // Nome do módulo (ex: "math")
    char** imported_names;          // Nomes importados (ex: ["sqrt", "abs"])
    int imported_count;             // Quantidade de nomes
    int import_all;                 // 1 = import math; 0 = from math import ...
} ImportStatementData;

typedef struct
{
    ASTNode* filename_expr;  // Expressão que retorna o nome do arquivo
} LoadExprData;

typedef struct
{
    ASTNode* expression;         // Expressão que retorna text
    char filename[BUFFER_SIZE];  // Nome do arquivo
} SaveStatementData;

// Dados para NODE_FUNCTION_CALL
typedef struct FunctionCallData
{
    char function_name[BUFFER_SIZE];  
    ASTNode** arguments;               
    int arg_count;                     
} FunctionCallData;

typedef struct ArrayIndexData
{
    ASTNode* array;      // Variável do array
    ASTNode* index;      // Expressão do índice
} ArrayIndexData;

typedef struct ASTNode
{
    NodeType type;
    int line;
    int column;
    
    union
    {
        BoolData                boolean;
        NumberData              number;
        StringData              string;
        VariableData            variable;
        BinaryOpData            binaryop;
        UnaryOpData             unaryop;
        LogicalOpData           logicalop; 
        NotOpData               notop;
        LetStatementData        let_stmt;   
        AssignmentData          assignment;
        StatementListData       stmt_list; 
        PrintStatementData      print_stmt; 
        ColorNodeData           color;
        AlignmentNodeData       alignment;
        WidthNodeData           width;
        InputStatementNode      input_stmt; 
        IfStatementData         if_stmt; 
        WhileStatementData      while_stmt; 
        BreakStatementData      break_stmt;
        ContinueStatementData   continue_stmt; 
        ForStatementData        for_stmt; 
        ImportStatementData     import_stmt;
        LoadExprData            load_expr;   
        SaveStatementData       save_stmt;
        FunctionCallData        function_call;
        ArrayIndexData          array_index;

    } data;

} ASTNode;


//===================================================================
// FUNCTIONS
//===================================================================
ASTNode* create_bool_node(int value, int line, int column);
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

ASTNode* create_assignment_node(ASTNode* target, ASTNode* value, 
                                int line, int column);

ASTNode* create_let_node(ASTNode* target, ASTNode* value, 
                         int line, int column);

ASTNode* create_statement_list_node(int line, int column);

void statement_list_add(ASTNode* list_node, ASTNode* stmt);

//ASTNode* create_input_node(const char* prompt, char* var_name, int line, int column);
ASTNode* create_input_node(ASTNode* color, ASTNode* width, ASTNode* alignment,
                          const char* prompt,
                          int set_nocolor,
                          char* var_name,
                          int line, int column);

// Funções para criar nó print
ASTNode* create_print_node(int line, int column);
ASTNode* create_color_node(TokenType color_token, int line, int column);
ASTNode* create_alignment_node(TokenType alignment_token, int line, int column);
ASTNode* create_width_node(int width_value, int line, int column);

// Operações de comparação e lógicas
ASTNode* create_comparison_op_node(LogicalOperator operator,
                                   ASTNode* left, ASTNode* right, 
                                   int line, int column);
ASTNode* create_logical_op_node(LogicalOperator operator,
                                ASTNode* left, ASTNode* right, 
                                int line, int column);
ASTNode* create_logical_not_node(ASTNode* operand, int line, int column);

// if
ASTNode* create_if_node(ASTNode* condition,
                        ASTNode* then_body, ASTNode* else_body,
                        int line, int column);


ASTNode* create_while_node(ASTNode* condition, ASTNode* body,
                           int line, int column);

ASTNode* create_break_node(int line, int column);

ASTNode* create_continue_node(int line, int column);

ASTNode* create_for_node(char var_name[],
                         ASTNode* init_value, 
                         ASTNode* end_value,
                         ASTNode* step_value, 
                         ASTNode* body,
                         int line, int column);

ASTNode* create_import_node(const char* module_name,
                            char** imported_names,
                            int imported_count,
                            int import_all,
                            int line, int column);

ASTNode* create_load_node(ASTNode* filename_expr, int line, int column);

ASTNode* create_save_node(ASTNode* expression, const char* filename, int line, int column);

ASTNode* create_function_call_node(const char* function_name, int line, int column);
void function_call_add_argument(ASTNode* node, ASTNode* argument);

ASTNode* create_array_node(int line, int column);
ASTNode* create_array_access_node(ASTNode* array, ASTNode* index, int line, int column);

ASTNode* create_empty_node(int line, int column);

void print_node_add_item(ASTNode* print_node, ASTNode* expr_node);
void print_set_newline(ASTNode* print_node, int has_newline);

void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif // AST_H
// Fim de st.h