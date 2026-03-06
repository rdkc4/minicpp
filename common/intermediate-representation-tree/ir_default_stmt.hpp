#ifndef IR_DEFAULT_STMT_HPP
#define IR_DEFAULT_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_switch_block_stmt.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRDefaultStmt
 * @brief IRT representation of the default statement node
*/
class IRDefaultStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt default case
     * @param ntype - type of the irt node
    */
    IRDefaultStmt(IRNodeType ntype);

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
    */
    void setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> block);

private:
    /// pointer to the switch-block of the default case
    std::unique_ptr<IRSwitchBlockStmt> swBlock;
};

#endif
