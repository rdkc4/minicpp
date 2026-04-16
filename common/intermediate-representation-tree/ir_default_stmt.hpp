#ifndef IR_DEFAULT_STMT_HPP
#define IR_DEFAULT_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_switch_block_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace IR::node {
    /**
     * @class IRDefaultStmt
     * @brief IRT representation of the default statement node
    */
    class IRDefaultStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt default case
        */
        IRDefaultStmt();

        /**
         * @brief getter for the switch block of the default statement
         * @returns pointer or const pointer to the switch block node
        */
        template<typename Self>
        decltype(auto) getSwitchBlockStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).switchBlockStmt.get();
        }

        /**
         * @brief initializes the default statement node
         * @param swBlockStmt - pointer to a switch block node
        */
        void setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> swBlockStmt);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IR::visitor::IRVisitor& visitor) override;

    private:
        /// pointer to the switch-block of the default case
        std::unique_ptr<IRSwitchBlockStmt> switchBlockStmt;
        
    };

}

#endif
