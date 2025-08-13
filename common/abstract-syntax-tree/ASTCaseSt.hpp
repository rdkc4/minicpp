#ifndef ASTCASE_ST_HPP
#define ASTCASE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTLiteral.hpp"
#include "ASTSwitchBlock.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTCaseSt final : public ASTStatement {
public:
    ASTCaseSt(const Token& token, ASTNodeType ntype);

    const ASTLiteral* getLiteral() const noexcept;

    const ASTSwitchBlock* getSwitchBlock() const noexcept;

    void setCase(std::unique_ptr<ASTLiteral> lit, std::unique_ptr<ASTSwitchBlock> _swBlock, bool hasBr);

    bool hasBreak() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTLiteral> literal;
    std::unique_ptr<ASTSwitchBlock> swBlock;
    bool _break;

};

#endif