#ifndef ASTIF_ST_HPP
#define ASTIF_ST_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"


class ASTIfSt final : public ASTStatement {
public:
    ASTIfSt(const Token& token, ASTNodeType ntype);

    const std::vector<std::unique_ptr<ASTExpression>>& getConditions() const noexcept;

    const std::vector<std::unique_ptr<ASTStatement>>& getStatements() const noexcept;

    void addIf(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTStatement> statement);

    void addElse(std::unique_ptr<ASTStatement> statement);

    bool hasElse() const noexcept;

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<ASTExpression>> conditions;
    std::vector<std::unique_ptr<ASTStatement>> statements;

};

#endif