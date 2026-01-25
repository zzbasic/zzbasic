#ifndef AST_H
#define AST_H

#include "zzdefs.h"

//===================================================================
// ESTRUTURAS (ENUMS E STRUCTS)
//===================================================================

typedef struct ASTNode ASTNode;

typedef enum
{
    NODE_NULL,
    NODE_NUMBER,
    NODE_STRING,       
    NODE_VARIABLE,    
    NODE_BINARY_OP,     // operacao binaria (+, -, *, /)
    NODE_UNARY_OP,      // operacao unaria (+, -)
    NODE_ASSIGNMENT,    // atribuição  
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

typedef struct Variable
{
    char name[VARNAME_SIZE];
    VariableType type; // TYPE_NULL = não inicializada
    
    union {
        double number;
        char string[STRING_SIZE];
        // Tipos futuros...
        // int boolean;
        // void* array;
        // void* function;
    } value;
} Variable;

typedef struct
{
    double value;
} NumberData;

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
    ASTNode* value;  // ASTNode que contém a expressão a ser atribuída
} AssignmentData;

typedef struct
{
    char var_name[VARNAME_SIZE];
} VariableData;

typedef struct
{
    char value[STRING_SIZE];
} StringData;

typedef struct ASTNode
{
    NodeType type;
    int line;
    int column;
    
    union {
        NumberData      number;
        StringData      string;
        VariableData    variable;
        BinaryOpData    binaryop;
        UnaryOpData     unaryop;
        AssignmentData  assignment;
    } data;
} ASTNode;


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

void free_ast(ASTNode* node);

void print_ast(ASTNode* node, int indent);

#endif // AST_H