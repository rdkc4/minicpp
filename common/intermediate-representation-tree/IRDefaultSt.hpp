#ifndef IRDEFAULT_ST_HPP
#define IRDEFAULT_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRSwitchBlock.hpp"
#include "defs/IRTree_defs.hpp"

class IRDefaultSt final : public IRStatement {
public:
    IRDefaultSt(IRNodeType ntype);

    template<typename Self>
    decltype(auto) getSwitchBlock(this Self&& self) noexcept {
        return std::forward<Self>(self).swBlock.get();
    }

    void setSwitchBlock(std::unique_ptr<IRSwitchBlock> block);

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRSwitchBlock> swBlock;
};

#endif