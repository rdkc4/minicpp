#ifndef EXPRESSION_CODE_GENERATOR_HPP
#define EXPRESSION_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/ir_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_id_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_literal_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_function_call_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_temporary_expr.hpp"

/** 
 * @class ExpressionCodeGenerator
 * @brief code generator specialized for expressions
*/
class ExpressionCodeGenerator {
public:
    /** 
     * @brief Creates the instance of the expression code generator
    */
    ExpressionCodeGenerator();

    /** 
     * @brief generates the asm code for the numerical expression
     * @param _numexp - const pointer to the irt numerical expression
    */
    void generateNumericalExpression(const IRExpr* _numexp);

    /** 
     * @brief generates the asm code for the relational expression
     * @param _relexp - const pointer to the irt relational expression (binary expression only at the moment)
    */
    void generateRelationalExpression(const IRExpr* _relexp);

    /** 
     * @brief gets the address of the variable on the stack
     * @param _id - const pointer to the irt id
     * @returns address of the variable on the stack
    */
    const std::string& generateID(const IRIdExpr* _id) const;

    /** 
     * @brief gets the literal
     * @param _literal - const pointer to irt literal
     * @returns literal
    */
    std::string generateLiteral(const IRLiteralExpr* _literal) const;

    /** 
     * @brief generates the asm code for assigning values to temporary variables
     * @param _temporary - const irt pointer to the irt temporary
    */
    void generateTemporaries(const IRTemporaryExpr* _temporary);

    /** 
     * @brief generates the asm code for the function call
     * @param _functionCall - const pointer to the irt function call
    */
    void generateFunctionCall(const IRFunctionCallExpr* _functionCall);

    /** 
     * @brief generates the asm code for the arguments of the function call
     * @param _functionCall - const pointer to the irt function call
    */
    void generateArgument(const IRFunctionCallExpr* _functionCall);

    /** 
     * @brief generates the asm code for removing arguments from the stack after function call is done
     * @param _arguments - number of arguments
    */
    void clearArguments(size_t _arguments);

};

#endif