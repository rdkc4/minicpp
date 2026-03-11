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
    ExpressionCodeGenerator() = default;

    /** 
     * @brief generates the asm code for the numerical expression
     * @param numericalExpr - const pointer to the irt numerical expression
    */
    void generateNumericalExpr(const IRExpr* numericalExpr);

    /** 
     * @brief generates the asm code for the binary expression
     * @param binaryExpr - const pointer to the irt binary expression
    */
    void generateBinaryExpr(const IRBinaryExpr* binaryExpr);

    /** 
     * @brief generates the asm code for the relational expression
     * @param relationalExpr - const pointer to the irt relational expression (binary expression only at the moment)
    */
    void generateRelationalExpr(const IRExpr* relationalExpr);

    /** 
     * @brief gets the address of the variable on the stack
     * @param idExpr - const pointer to the irt id
     * @returns address of the variable on the stack
    */
    const std::string& generateIDExpr(const IRIdExpr* idExpr) const;

    /** 
     * @brief gets the literal
     * @param literalExpr - const pointer to irt literal
     * @returns literal
    */
    std::string generateLiteralExpr(const IRLiteralExpr* literalExpr) const;

    /** 
     * @brief generates the asm code for assigning values to temporary variables
     * @param temporaryExpr - const irt pointer to the irt temporary
    */
    void generateTemporaryExprs(const IRTemporaryExpr* temporaryExpr);

    /** 
     * @brief generates the asm code for the function call
     * @param callExpr - const pointer to the irt function call
    */
    void generateFunctionCallExpr(const IRFunctionCallExpr* callExpr);

    /** 
     * @brief generates the asm code for the arguments of the function call
     * @param callExpr - const pointer to the irt function call
    */
    void generateArguments(const IRFunctionCallExpr* callExpr);

    /** 
     * @brief generates the asm code for removing arguments from the stack after function call is done
     * @param argc - number of arguments
    */
    void clearArguments(size_t argc);

};

#endif