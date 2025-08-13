#ifndef ASTFOR_ST_HPP
#define ASTFOR_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTAssignSt.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"


class ASTForSt final : public ASTStatement {
public:
    ASTForSt(const Token& token, ASTNodeType ntype);

    const ASTAssignSt* getInitializer() const noexcept;

    const ASTExpression* getCondition() const noexcept;

    ASTExpression* getConditionNC() const noexcept;

    const ASTAssignSt* getIncrementer() const noexcept;
    
    const ASTStatement* getStatement() const noexcept;

    void setForSt(std::unique_ptr<ASTAssignSt> init, std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTAssignSt> inc, std::unique_ptr<ASTStatement> st);

    bool hasInitializer() const noexcept;

    bool hasCondition() const noexcept;

    bool hasIncrementer() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTAssignSt> initializer;
    std::unique_ptr<ASTExpression> condition;
    std::unique_ptr<ASTAssignSt> incrementer;
    std::unique_ptr<ASTStatement> statement;

};

#endif