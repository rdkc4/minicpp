#ifndef ASTDOWHILE_ST_HPP
#define ASTDOWHILE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTDoWhileSt final : public ASTStatement {
public:
    ASTDoWhileSt(const Token& token, ASTNodeType ntype);

    const ASTExpression* getCondition() const noexcept;

    ASTExpression* getConditionNC() const noexcept;

    const ASTStatement* getStatement() const noexcept;

    void setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st);

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> condition;
    std::unique_ptr<ASTStatement> statement;

};

#endif