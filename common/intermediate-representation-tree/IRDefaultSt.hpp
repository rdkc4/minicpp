#ifndef IRDEFAULT_ST_HPP
#define IRDEFAULT_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRSwitchBlock.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRDefaultSt
 * @brief IRT representation of the default statement node
*/
class IRDefaultSt final : public IRStatement {
public:
    IRDefaultSt(IRNodeType ntype);

    /**
     * @brief getter for the switch block of the default statement
     * @returns pointer or const pointer to the switch block node
    */
    template<typename Self>
    decltype(auto) getSwitchBlock(this Self&& self) noexcept {
        return std::forward<Self>(self).swBlock.get();
    }

    /**
     * @brief initializes the default statement node
     * @param block - pointer to a switch block node
     * @returns void
    */
    void setSwitchBlock(std::unique_ptr<IRSwitchBlock> block);

    /**
     * @brief prints formatted string of the default statement node
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRSwitchBlock> swBlock;
};

#endif
