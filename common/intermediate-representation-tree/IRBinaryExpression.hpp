#ifndef IRBINARY_EXPRESSION_HPP
#define IRBINARY_EXPRESSION_HPP

#include <memory>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRBinaryExpression
 * @brief IRT representation of the binary expression node
*/
class IRBinaryExpression final : public IRExpression {
public:
    IRBinaryExpression(IRNodeType ntype, Types type);

    /**
     * @brief getter for the left operand of the binary expression
     * @returns const pointer to the expression node
    */
    const IRExpression* getLeftOperand() const noexcept;

    /**
     * @brief getter for the right operand of the binary expression 
     * @returns const pointer to the expression node
    */
    const IRExpression* getRightOperand() const noexcept;

    /**
     * @brief getter for the operator of the binary expression
     * @returns operator of the binary expression
    */
    Operators getOperator() const noexcept;

    /**
     * @brief initializes the binary expression node
     * @param lOp - pointer to the left operand
     * @param rOp - pointer to the right operand
     * @param _op - operator of the binary expression
     * @returns void
    */
    void setBinaryExpression(std::unique_ptr<IRExpression> lOp, std::unique_ptr<IRExpression> rOp, Operators _op);

    /**
     * @brief prints formatted string of the binary expression node
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> leftOperand;
    std::unique_ptr<IRExpression> rightOperand;
    Operators op;
};

#endif
