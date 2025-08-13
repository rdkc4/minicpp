#ifndef ASTCOMPOUND_ST_HPP
#define ASTCOMPOUND_ST_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTCompoundSt final : public ASTStatement {
public:
    ASTCompoundSt(const Token& token, ASTNodeType ntype);

    const std::vector<std::unique_ptr<ASTStatement>>& getStatements() const noexcept;

    void addStatement(std::unique_ptr<ASTStatement> statement);

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<ASTStatement>> statements;

};

#endif