#ifndef IRCASE_ST_HPP
#define IRCASE_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRLiteral.hpp"
#include "IRSwitchBlock.hpp"
#include "defs/IRTree_defs.hpp"

class IRCaseSt final : public IRStatement {
public:
    IRCaseSt(IRNodeType ntype);

    const IRLiteral* getLiteral() const noexcept;

    const IRSwitchBlock* getSwitchBlock() const noexcept;

    IRSwitchBlock* getSwitchBlockNC() const noexcept;

    void setCase(std::unique_ptr<IRLiteral> lit, std::unique_ptr<IRSwitchBlock> block, bool hasBr);

    bool hasBreak() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRLiteral> literal;
    std::unique_ptr<IRSwitchBlock> swBlock;
    bool breaks;
};


#endif