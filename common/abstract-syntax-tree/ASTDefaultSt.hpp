#ifndef ASTDEFAULT_ST_HPP
#define ASTDEFAULT_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTSwitchBlock.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTDefaultSt final : public ASTStatement {
public:
    ASTDefaultSt(const Token& token, ASTNodeType ntype);

    const ASTSwitchBlock* getSwitchBlock() const noexcept;

    void setDefault(std::unique_ptr<ASTSwitchBlock> _swBlock);

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTSwitchBlock> swBlock;

};

#endif