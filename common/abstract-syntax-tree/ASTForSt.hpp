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

    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

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