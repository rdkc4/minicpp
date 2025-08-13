#ifndef ASTBINARY_EXPRESSION_HPP
#define ASTBINARY_EXPRESSION_HPP

#include <memory>

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTBinaryExpression final : public ASTExpression {
public:
    ASTBinaryExpression(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE, Operators op = Operators::NO_OP);
    
    const ASTExpression* getLeftOperand() const noexcept;

    ASTExpression* getLeftOperandNC() noexcept;

    const ASTExpression* getRightOperand() const noexcept;

    ASTExpression* getRightOperandNC() noexcept;

    Operators getOperator() const noexcept;

    void setOperator(Operators _op) noexcept;

    void setOperands(std::unique_ptr<ASTExpression> lOp, std::unique_ptr<ASTExpression> rOp);

    bool initialized() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> leftOperand;
    std::unique_ptr<ASTExpression> rightOperand;
    Operators op;
};


#endif