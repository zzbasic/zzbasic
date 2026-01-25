// evaluator.c

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>  

#include "color.h"
#include "a89alloc.h"
#include "evaluator.h"

#define EPSILON 1e-12

// ============================================
// HELPER FUNCTIONS (static)
// ============================================

static EvaluatorResult create_success_result_number(double value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.success = 1;
    result.is_string = 0;
    result.value.number = value;
    result.line = line;      
    result.column = column;  
    return result;
}

static EvaluatorResult create_success_result_string(const char* value, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    result.success = 1;
    result.is_string = 1;
    strncpy(result.value.string, value, sizeof(result.value.string) - 1);
    result.value.string[sizeof(result.value.string) - 1] = '\0';
    result.line = line;
    result.column = column;
    return result;
}

static EvaluatorResult create_error_result(const char* message, int line, int column)
{
    EvaluatorResult result;
    memset(&result, 0, sizeof(EvaluatorResult)); 
    snprintf(result.error_message, sizeof(result.error_message), 
             "%s[%d:%d] %s%s", COLOR_ERROR, line, column, message, COLOR_RESET);
    result.line = line;
    result.column = column;
    return result;
}

// printf-style version for formatted messages
static EvaluatorResult create_error_result_fmt(int line, int column, 
                                              const char* format, ...)
{
    char message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    return create_error_result(message, line, column);
}

// ============================================
// EVALUATE EXPRESSIONS (with context)
// ============================================

EvaluatorResult evaluate_expression(ASTNode* node, SymbolTable* symbols, EvalContext ctx)
{
    if (node == NULL)
    {
        return create_error_result("Error: AST node is null", 0, 0);
    }
    
    switch (node->type)
    {
        case NODE_NUMBER:
        {
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Number cannot be used as string");
            }
            return create_success_result_number(node->data.number.value, 
                                               node->line, node->column);
        }
            
        case NODE_VARIABLE:
        {
            const char* var_name = node->data.variable.var_name;
            
            // Check if exists
            if (!symbol_table_exists(symbols, var_name))
            {
                return create_error_result_fmt(node->line, node->column,
                     "Variable '%s' not declared. Use 'let %s = value'", 
                     var_name, var_name);
            }
            
            // Try as number
            double num_value;
            if (symbol_table_get_number(symbols, var_name, &num_value))
            {
                if (ctx == CTX_STRING)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Variable '%s' is a number, cannot be used as string", 
                         var_name);
                }
                return create_success_result_number(num_value, node->line, node->column);
            }
            
            // Try as string
            char str_value[STRING_SIZE];
            if (symbol_table_get_string(symbols, var_name, str_value, sizeof(str_value)))
            {
                if (ctx == CTX_NUMBER)
                {
                    return create_error_result_fmt(node->line, node->column,
                         "Variable '%s' is a string, cannot be used in mathematical operation", 
                         var_name);
                }
                return create_success_result_string(str_value, node->line, node->column);
            }
            
            // Should not reach here
            return create_error_result_fmt(node->line, node->column,
                 "Internal error: unknown variable type '%s'", var_name);
        }
            
        case NODE_BINARY_OP:
        {
            // Binary operations always expect numbers (for now)
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Mathematical operation cannot be used as string");
            }
            
            // Evaluate operands in number context
            EvaluatorResult left_result = evaluate_expression(
                node->data.binaryop.left, symbols, CTX_NUMBER);
            if (!left_result.success) return left_result;
            
            EvaluatorResult right_result = evaluate_expression(
                node->data.binaryop.right, symbols, CTX_NUMBER);
            if (!right_result.success) return right_result;
            
            // Both must be numbers
            if (left_result.is_string || right_result.is_string)
            {
                return create_error_result("Mathematical operation with string", 
                                         node->line, node->column);
            }
            
            double result;
            switch (node->data.binaryop.operator)
            {
                case '+': 
                    result = left_result.value.number + right_result.value.number; 
                    break;
                case '-': 
                    result = left_result.value.number - right_result.value.number; 
                    break;
                case '*': 
                    result = left_result.value.number * right_result.value.number; 
                    break;
                case '/': 
                    if (fabs(right_result.value.number) < EPSILON)
                    {
                        return create_error_result_fmt(node->line, node->column,
                             "Division by zero");
                    }
                    result = left_result.value.number / right_result.value.number; 
                    break;
                default: 
                    return create_error_result_fmt(node->line, node->column,
                         "Invalid operator '%c'", node->data.binaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_UNARY_OP:
        {
            // Unary operations always expect numbers
            if (ctx == CTX_STRING)
            {
                return create_error_result_fmt(node->line, node->column,
                     "Unary operator cannot be applied to string");
            }
            
            EvaluatorResult operand_result = evaluate_expression(
                node->data.unaryop.operand, symbols, CTX_NUMBER);
            if (!operand_result.success) return operand_result;
            
            if (operand_result.is_string)
            {
                return create_error_result("Unary operator '-' applied to string", 
                                         node->line, node->column);
            }
            
            double result;
            switch (node->data.unaryop.operator)
            {
                case '+':
                    result = operand_result.value.number;  // +x = x
                    break;
                case '-':
                    result = -operand_result.value.number; // -x
                    break;
                default:
                    return create_error_result_fmt(node->line, node->column,
                         "Invalid unary operator '%c'", node->data.unaryop.operator);
            }
            
            return create_success_result_number(result, node->line, node->column);
        }
            
        case NODE_STRING:
        {
            if (ctx == CTX_NUMBER)
            {
                return create_error_result_fmt(node->line, node->column,
                     "String cannot be used as number");
            }
            return create_success_result_string(node->data.string.value, 
                                               node->line, node->column);
        }
            
        default:
            return create_error_result_fmt(node->line, node->column,
                 "Unsupported node type: %d", node->type);
    }
}

// ============================================
// EXECUTE STATEMENT (uses CTX_ANY by default)
// ============================================

int execute_statement(ASTNode* node, SymbolTable* symbols)
{
    if (!node) return 0;
    
    switch (node->type)
    {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->data.assignment.var_name;
            ASTNode* value_node = node->data.assignment.value;
            
            // Evaluate value (any type)
            EvaluatorResult value_result = evaluate_expression(
                value_node, symbols, CTX_ANY);
            
            if (!value_result.success)
            {
                printf("%s\n", value_result.error_message);
                return 0;
            }
            
            // Store
            if (value_result.is_string)
            {
                if (!symbol_table_set_string(symbols, var_name, value_result.value.string))
                {
                    printf("Error assigning string to '%s'\n", var_name);
                    return 0;
                }
                printf("OK: %s = \"%s\"\n", var_name, value_result.value.string);
            }
            else
            {
                if (!symbol_table_set_number(symbols, var_name, value_result.value.number))
                {
                    printf("Error assigning number to '%s'\n", var_name);
                    return 0;
                }
                printf("OK: %s = %g\n", var_name, value_result.value.number);
            }
            return 1;
        }
            
        case NODE_NUMBER:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_VARIABLE:
        {
            // Evaluate for display (any type)
            EvaluatorResult result = evaluate_expression(node, symbols, CTX_ANY);
            if (result.success)
            {
                if (result.is_string)
                {
                    printf("= \"%s\"\n", result.value.string);
                }
                else
                {
                    printf("= %g\n", result.value.number);
                }
                return 1;
            }
            else
            {
                printf("%s\n", result.error_message);
                return 0;
            }
        }
            
        case NODE_STRING:
        {
            // Standalone string
            printf("= \"%s\"\n", node->data.string.value);
            return 1;
        }
            
        default:
            printf("Unsupported statement type: %d\n", node->type);
            return 0;
    }
}

// ============================================
// EVALUATE PROGRAM (compatibility)
// ============================================

int evaluate_program(ASTNode* node, SymbolTable* symbols)
{
    return execute_statement(node, symbols);
}

// Old function (for compatibility)
EvaluatorResult evaluate(ASTNode* node)
{
    SymbolTable* symbols = symbol_table_create();
    if (!symbols)
    {
        return create_error_result("Error: could not create symbol table", 0, 0);
    }
    
    EvaluatorResult result = evaluate_expression(node, symbols, CTX_ANY);
    symbol_table_destroy(symbols);
    return result;
}


//===================================================================
// TEST
//===================================================================
#ifdef TESTEVALUATOR
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"

void test_expression(const char* expr, const char* expected_msg)
{
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sTeste: %s%s\n", YELLOW, expr, COLOR_RESET);
    
    if (expected_msg)
    {
        printf("%sEsperado: %s%s\n", COLOR_INFO, expected_msg, COLOR_RESET);
    }
    
    Lexer lexer;
    lexer_init(&lexer, expr);
    
    ASTNode* ast = parse_single_statement(&lexer);
    if (ast == NULL)
    {
        printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        return;
    }
    
    printf("%sAST gerada:%s\n", COLOR_INFO, COLOR_RESET);
    print_ast(ast, 1);
    
    // Create symbol table for test
    SymbolTable* symbols = symbol_table_create();
    
    // FOR SIMPLE EXPRESSIONS, USE CTX_ANY (accepts any type)
    EvaluatorResult result = evaluate_expression(ast, symbols, CTX_ANY);
    
    if (result.success)
    {
        if (result.is_string)
        {
            printf("%sResultado: \"%s\"%s\n", COLOR_SUCCESS, result.value.string, COLOR_RESET);
        }
        else
        {
            printf("%sResultado: %g%s\n", COLOR_SUCCESS, result.value.number, COLOR_RESET);
        }
    }
    else
    {
        printf("%s✗ Erro: %s%s\n", COLOR_ERROR, result.error_message, COLOR_RESET);
    }
    
    free_ast(ast);
    symbol_table_destroy(symbols);
    wait();
}

void test_with_context(const char* setup_code, const char* test_code, 
                       const char* expected_msg, EvalContext ctx)
{
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sContexto: %s%s\n", YELLOW, setup_code, COLOR_RESET);
    printf("%sTeste: %s%s\n", YELLOW, test_code, COLOR_RESET);
    printf("%sContexto de avaliação: %s%s\n", COLOR_INFO, 
           ctx == CTX_ANY ? "QUALQUER" : 
           ctx == CTX_NUMBER ? "NÚMERO" : "STRING", 
           COLOR_RESET);
    
    SymbolTable* symbols = symbol_table_create();
    
    // Execute setup
    if (setup_code && setup_code[0] != '\0')
    {
        Lexer setup_lexer;
        lexer_init(&setup_lexer, setup_code);
        ASTNode* setup_ast = parse_single_statement(&setup_lexer);
        if (setup_ast)
        {
            execute_statement(setup_ast, symbols);
            free_ast(setup_ast);
        }
    }
    
    // Execute test
    Lexer test_lexer;
    lexer_init(&test_lexer, test_code);
    ASTNode* test_ast = parse_single_statement(&test_lexer);
    
    if (test_ast == NULL)
    {
        printf("%s✗ Erro no parsing do teste%s\n", COLOR_ERROR, COLOR_RESET);
        symbol_table_destroy(symbols);
        return;
    }
    
    printf("%sAST do teste:%s\n", COLOR_INFO, COLOR_RESET);
    print_ast(test_ast, 1);
    
    // Use provided context
    EvaluatorResult result = evaluate_expression(test_ast, symbols, ctx);
    
    if (result.success)
    {
        if (result.is_string)
        {
            printf("%sResultado: \"%s\"%s\n", COLOR_SUCCESS, result.value.string, COLOR_RESET);
        }
        else
        {
            printf("%sResultado: %g%s\n", COLOR_SUCCESS, result.value.number, COLOR_RESET);
        }
    }
    else
    {
        printf("%s✗ Erro: %s%s\n", COLOR_ERROR, result.error_message, COLOR_RESET);
    }
    
    // Show symbol table
    printf("\n%sTabela de símbolos após teste:%s\n", COLOR_INFO, COLOR_RESET);
    symbol_table_print(symbols);
    
    free_ast(test_ast);
    symbol_table_destroy(symbols);
    wait();
}

int main(void) {
    setup_utf8();
    
    printf("%s┌───────────────────────────────────────┐%s\n", CYAN, COLOR_RESET);
    printf("%s│   EVALUATOR ZzBasic v0.2.0            │%s\n", CYAN, COLOR_RESET);
    printf("%s│   COM CONTEXTO DE TIPOS               │%s\n", CYAN, COLOR_RESET);
    printf("%s└───────────────────────────────────────┘%s\n\n", CYAN, COLOR_RESET);
    
    // ============================================
    // TESTE 1: Basic expressions (no variables)
    // ============================================
    printf("%s1. BASIC EXPRESSIONS (CTX_NUMBER):%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("", "5 + 3", "8", CTX_NUMBER);
    test_with_context("", "10 - 4 * 2", "2", CTX_NUMBER);
    test_with_context("", "(5 + 3) * 2", "16", CTX_NUMBER);
    test_with_context("", "10 / 2 + 1", "6", CTX_NUMBER);
    test_with_context("", "-5 + 3", "-2", CTX_NUMBER);
    test_with_context("", "-(5 + 3)", "-8", CTX_NUMBER);
    test_with_context("", "5 * -2", "-10", CTX_NUMBER);
    
    // ============================================
    // TESTE 2: Basic strings
    // ============================================
    printf("\n%s2. BASIC STRINGS (CTX_ANY):%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("", "\"Olá Mundo\"", "Olá Mundo", CTX_ANY);
    test_with_context("", "\"\"", "string vazia", CTX_ANY);
    
    // Test: string in numeric context (MUST FAIL)
    printf("\n%s3. STRING IN NUMERIC CONTEXT (EXPECTED ERROR):%s\n", COLOR_HEADER, COLOR_RESET);
    test_with_context("", "\"123\"", "Erro: string não pode ser usada como número", CTX_NUMBER);
    
    // ============================================
    // TESTE 4: Numeric variables
    // ============================================
    printf("\n%s4. NUMERIC VARIABLES:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("let x = 10", "x", "10", CTX_ANY);
    test_with_context("let x = 10", "x + 5", "15", CTX_NUMBER);
    test_with_context("let a = 5", "let b = 3", "b = 3", CTX_ANY);  // Assignment
    test_with_context("let a = 5\nlet b = 3", "a * b", "15", CTX_NUMBER);
    test_with_context("let total = 100", "total / 4", "25", CTX_NUMBER);
    
    // ============================================
    // TESTE 5: String variables
    // ============================================
    printf("\n%s5. STRING VARIABLES:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("let nome = \"João\"", "nome", "João", CTX_ANY);
    test_with_context("let msg = \"Olá\"", "msg", "Olá", CTX_ANY);
    
    // ============================================
    // TESTE 6: Type errors
    // ============================================
    printf("\n%s6. TYPE ERRORS (CONTEXT CHECKING):%s\n", COLOR_HEADER, COLOR_RESET);
    
    // String in mathematical operation
    test_with_context("let texto = \"123\"", "texto + 5", 
                     "Erro: string em operação matemática", CTX_NUMBER);
    
    // Number where string is expected (future: concatenation)
    test_with_context("let numero = 456", "numero", 
                     "456 (mas em CTX_STRING daria erro)", CTX_ANY);
    
    // ============================================
    // TESTE 7: Case-sensitive
    // ============================================
    printf("\n%s7. CASE-SENSITIVE:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("let Total = 100\nlet total = 50", "Total", "100", CTX_ANY);
    test_with_context("let Total = 100\nlet total = 50", "total", "50", CTX_ANY);
    test_with_context("let Total = 100", "TOTAL", 
                     "Erro: variável não declarada", CTX_ANY);
    
    // ============================================
    // TESTE 8: Errors - Undeclared variables
    // ============================================
    printf("\n%s8. UNDECLARED VARIABLES:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("", "variavel_inexistente", 
                     "Erro: variável não declarada", CTX_ANY);
    test_with_context("", "x + 10", 
                     "Erro: x não declarada", CTX_NUMBER);
    
    // ============================================
    // TESTE 9: Division by zero
    // ============================================
    printf("\n%s9. DIVISION BY ZERO:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("", "10 / 0", "Erro: divisão por zero", CTX_NUMBER);
    test_with_context("let zero = 0", "5 / zero", 
                     "Erro: divisão por zero", CTX_NUMBER);
    
    // ============================================
    // TESTE 10: Complex expressions
    // ============================================
    printf("\n%s10. COMPLEX EXPRESSIONS:%s\n", COLOR_HEADER, COLOR_RESET);
    
    test_with_context("let preco = 10.5\nlet quantidade = 3", 
                     "(preco * quantidade) * 1.1", "34.65", CTX_NUMBER);
    test_with_context("let a = 2\nlet b = 3\nlet c = 4", 
                     "a * b + c / 2", "8", CTX_NUMBER);
    
    printf("\n%s═══════════════════════════════════════════%s\n", CYAN, COLOR_RESET);
    printf("%sVerificação de vazamento de memória:%s\n", YELLOW, COLOR_RESET);
    a89check_leaks();
    
    printf("\n%s✅ Todos os testes executados!%s\n", COLOR_SUCCESS, COLOR_RESET);
    
    return 0;
}
#endif

// END OF evaluator.c