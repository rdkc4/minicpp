#ifndef ASTPRINTF_ST_HPP
#define ASTPRINTF_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTPrintfSt final : public ASTStatement {
public:
    ASTPrintfSt(const Token& token, ASTNodeType ntype);

    const ASTExpression* getExp() const noexcept;

    ASTExpression* getExpNC() const noexcept;

    void setExp(std::unique_ptr<ASTExpression> nexp);

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> exp;

};


#endif