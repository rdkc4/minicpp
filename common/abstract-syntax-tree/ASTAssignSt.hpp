#ifndef ASTASSIGN_ST_HPP
#define ASTASSIGN_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTId.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTAssignSt final : public ASTStatement {
public:
    ASTAssignSt(const Token& token, ASTNodeType ntype);

    const ASTId* getVariable() const noexcept;

    ASTId* getVariableNC() const noexcept;

    const ASTExpression* getExp() const noexcept;

    ASTExpression* getExpNC() const noexcept;

    void setVariable(std::unique_ptr<ASTId> var);

    void setExp(std::unique_ptr<ASTExpression> nexp);

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTId> variable;
    std::unique_ptr<ASTExpression> exp;

};


#endif