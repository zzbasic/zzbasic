#include <stdio.h>
#include <string.h>
#include <math.h>

#include "evaluator.h"
#include "a89alloc.h"

#define EPSILON 1e-12

//===================================================================
// CRIA RESULTADOS
//===================================================================
static EvaluatorResult create_success_result_number(double value, int line, int column)
{
    EvaluatorResult result;
    result.success = 1;
    result.is_string = 0;
    result.value.number_value = value;
    result.value.string_value[0] = '\0';
    result.error_message[0] = '\0';
    result.line = line;      
    result.column = column;  
    return result;
}

static EvaluatorResult create_error_result(const char* message, int line, int column)
{
    EvaluatorResult result;
    result.success = 0;
    result.is_string = 0;
    result.value.number_value = 0;
    strncpy(result.error_message, message, sizeof(result.error_message) - 1);
    result.error_message[sizeof(result.error_message) - 1] = '\0';
    result.line = line;
    result.column = column;
    return result;
}


//===================================================================
// AVALIA A AST
//===================================================================
EvaluatorResult evaluate(ASTNode* node) {
    if (node == NULL) return create_error_result("AST node null", 0, 0);
        
    switch (node->type) {
        case NODE_NUMBER:
            return create_success_result_number(node->value, node->line, node->column);  

        case NODE_BINARY_OP:{            
                EvaluatorResult left_result = evaluate(node->left);
                if (!left_result.success) {
                    return left_result;
                }
                
                EvaluatorResult right_result = evaluate(node->right);
                if (!right_result.success) {
                    return right_result;
                }
                
                double result;
                switch (node->operator) {
                    case '+': result = left_result.value.number_value + right_result.value.number_value; break;
                    case '-': result = left_result.value.number_value - right_result.value.number_value; break;
                    case '*': result = left_result.value.number_value * right_result.value.number_value; break;
                    case '/': 
                        if (fabs(right_result.value.number_value) < EPSILON)
                        {                  
                            return create_error_result("Error: division by zero",
                                                       node->right->line, 
                                                       node->right->column);
                        }
                        result = left_result.value.number_value / right_result.value.number_value; 
                        break;
                    default: 
                        return create_error_result("Invalid binary operator", node->line, node->column);
                }
                
                return create_success_result_number(result, node->line, node->column);
        }   
        
        case NODE_UNARY_OP:
            {
                EvaluatorResult operand_result = evaluate(node->operand);  
                if (!operand_result.success) {
                    return operand_result;
                }          

                double result = -operand_result.value.number_value;

                return create_success_result_number(result, node->line, node->column);
            }
            
        default:
            return create_error_result("Unknown AST node type", node->line, node->column);
    }
}

