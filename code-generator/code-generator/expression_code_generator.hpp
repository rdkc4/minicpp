#ifndef EXPRESSION_CODE_GENERATOR_HPP
#define EXPRESSION_CODE_GENERATOR_HPP

#include <string_view>

#include "../../common/intermediate-representation-tree/ir_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_id_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_literal_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_function_call_expr.hpp"
#include "../../common/intermediate-representation-tree/ir_temporary_expr.hpp"
#include "defs/code_generator_defs.hpp"
#include "defs/binary_operands.hpp"
#include "ctx/code_generator_ctx.hpp"

/** 
 * @class ExpressionCodeGenerator
 * @brief code generator specialized for expressions
*/
class ExpressionCodeGenerator {
public:
    /** 
     * @brief Creates the instance of the expression code generator
     * @param context - reference to a context of the function
    */
    ExpressionCodeGenerator(CodeGeneratorFunctionContext& context);

    /** 
     * @brief generates the asm code for the expression
     * @param expr - const pointer to the irt expression
     * @param exprCtx - context of the expression, defaults to value
    */
    void generateExpr(
        const ir::IRExpr* expr, 
        ExprContext exprCtx = ExprContext::VALUE
    );

    /** 
     * @brief generates the asm code for the binary expression
     * @param binaryExpr - const pointer to the irt binary expression
     * @param exprCtx - context of the expression, defaults to value
    */
    void generateBinaryExpr(
        const ir::IRBinaryExpr* binaryExpr, 
        ExprContext exprCtx = ExprContext::VALUE
    );

    /** 
     * @brief generates the asm code for the relational expression
     * @param expr - const pointer to the irt expression
     * @param trueLabel - label when expression is true
     * @param falseLabel - label when expression is false
    */
    void generateConditionExpr(const ir::IRExpr* expr, std::string_view trueLabel, std::string_view falseLabel);

    /** 
     * @brief generates the asm code for the id expression
     * @param idExpr - const pointer to the irt id expression
    */
    void generateIdExpr(const ir::IRIdExpr* idExpr) const;

    /**
     * @brief getter for the address of the id
     * @param idExpr - const pointer to the id expression
     * @returns address of the id expression
    */
    std::string_view getIdExprAddress(const ir::IRIdExpr* idExpr) const;

    /** 
     * @brief generates literal expression
     * @param literalExpr - const pointer to irt literal expression
    */
    void generateLiteralExpr(const ir::IRLiteralExpr* literalExpr);

    /** 
     * @brief generates the format of the literal expression
     * @param literalExpr - const pointer to irt literal expression
     * @returns formatted literal
    */
    std::string formatLiteral(const ir::IRLiteralExpr* literalExpr) const;

    /** 
     * @brief generates the asm code for assigning values to temporary variables
     * @param temporaryExpr - const irt pointer to the irt temporary
    */
    void generateTemporaryExprs(const ir::IRTemporaryExpr* temporaryExpr);

    /** 
     * @brief generates the asm code for the function call
     * @param callExpr - const pointer to the irt function call
    */
    void generateFunctionCallExpr(const ir::IRFunctionCallExpr* callExpr, bool expectsReturnVal = true);

    /** 
     * @brief generates the asm code for the arguments of the function call
     * @param callExpr - const pointer to the irt function call
    */
    void generateArguments(const ir::IRFunctionCallExpr* callExpr);

    /** 
     * @brief generates the asm code for removing arguments from the stack after function call is done
     * @param argc - number of arguments
    */
    void clearArguments(size_t argc);

private:
    /// reference to a context of the function
    CodeGeneratorFunctionContext& ctx;

    /**
     * @brief getter for the operand
     * @param fallbackOperand - fallback operand when gp registers are unavailable
     * @returns name of the operand
    */
    std::string getUnaryOperand(std::string_view fallbackOperand);

    /**
     * @brief getter for the operands that are participating
     * in the binary operation
     * @returns binary operands
    */
    BinaryOperands getBinaryOperands();

    /**
     * @brief generates multiplication and division binary expressions
     * @param binaryExpr - const pointer to the irt binary expression
     * @param operands - left and right operand of the binary expression
    */
    void generateMultiplicativeExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands);

    /**
     * @brief generates shift binary expression
     * @param binaryExpr - const pointer to the irt binary expression
     * @param operands - left and right operand of the binary expression
    */
    void generateShiftExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands);

    /**
     * @brief generates logical and binary expression
     * @param operands - left and right operand of the binary expression
    */
    void generateLogicalAndExpr(BinaryOperands operands);

    /**
     * @brief generates logical or binary expression
     * @param operands - left and right operand of the binary expression
    */
    void generateLogicalOrExpr(BinaryOperands operands);

    /**
     * @brief generates relational binary expression
     * @param binaryExpr - const pointer to the binary expression
     * @param exprCtx - context of the expression, defaults to value
     * @param operands - left and right operand of the binary expression
    */
    void generateRelationalExpr(
        const ir::IRBinaryExpr* binaryExpr, 
        BinaryOperands operands, 
        ExprContext exprCtx = ExprContext::VALUE
    );

    /**
     * @brief generates ALU expressions (add, sub, and, or, xor)
     * @param binaryExpr - const pointer to the irt binary expression
     * @param operands - left and right operand of the binary expression
    */
    void generateALUExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands);

};

#endif