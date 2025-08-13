#ifndef IRBINARY_EXPRESSION_HPP
#define IRBINARY_EXPRESSION_HPP

#include <memory>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRBinaryExpression final : public IRExpression {
public:
    IRBinaryExpression(IRNodeType ntype, Types type);

    const IRExpression* getLeftOperand() const noexcept;
    const IRExpression* getRightOperand() const noexcept;
    Operators getOperator() const noexcept;

    void setBinaryExpression(std::unique_ptr<IRExpression> lOp, std::unique_ptr<IRExpression> rOp, Operators _op);

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> leftOperand;
    std::unique_ptr<IRExpression> rightOperand;
    Operators op;
};

#endif