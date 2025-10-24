#ifndef ASTWHILE_ST_HPP
#define ASTWHILE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTWhileSt final : public ASTStatement {
public:
    ASTWhileSt(const Token& token, ASTNodeType ntype);

    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    const ASTStatement* getStatement() const noexcept;

    void setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st);

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> condition;
    std::unique_ptr<ASTStatement> statement;

};

#endif