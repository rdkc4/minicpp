#ifndef IRSWITCH_BLOCK_HPP
#define IRSWITCH_BLOCK_HPP

#include <memory>
#include <vector>

#include "IRStatement.hpp"
#include "defs/IRTree_defs.hpp"

class IRSwitchBlock final : public IRStatement {
public:
    IRSwitchBlock(IRNodeType ntype);

    const std::vector<std::unique_ptr<IRStatement>>& getStatements() const noexcept;

    void addStatement(std::unique_ptr<IRStatement> statement);

    void eliminateDead(size_t startIdx);

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRStatement>> statements;
};

#endif