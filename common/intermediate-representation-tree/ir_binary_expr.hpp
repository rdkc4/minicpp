#ifndef IR_BINARY_EXPR_HPP
#define IR_BINARY_EXPR_HPP

#include <memory>

#include "ir_expr.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRBinaryExpr
 * @brief IRT representation of the binary expression node
*/
class IRBinaryExpr final : public IRExpr {
public:
    /** 
     * @brief Creates the instance of the irt binary expression
     * @param ntype - type of the irt node
     * @param type - type of the binary expression
    */
    IRBinaryExpr(IRNodeType ntype, Type type);

    /**
     * @brief getter for the left operand of the binary expression
     * @returns const pointer to the expression node
    */
    const IRExpr* getLeftOperand() const noexcept;

    /**
     * @brief getter for the right operand of the binary expression 
     * @returns const pointer to the expression node
    */
    const IRExpr* getRightOperand() const noexcept;

    /**
     * @brief getter for the operator of the binary expression
     * @returns operator of the binary expression
    */
    Operator getOperator() const noexcept;

    /**
     * @brief initializes the binary expression node
     * @param lOp - pointer to the left operand
     * @param rOp - pointer to the right operand
     * @param _op - operator of the binary expression
    */
    void setBinaryExpression(std::unique_ptr<IRExpr> lOp, std::unique_ptr<IRExpr> rOp, Operator _op);

private:
    /// pointer to the left operand of the binary expression
    std::unique_ptr<IRExpr> leftOperand;

    /// pointer to the right operand of the binary expression
    std::unique_ptr<IRExpr> rightOperand;
    
    /// operator of the binary expression
    Operator op;
};

#endif
