#ifndef IR_SWITCH_BLOCK_STMT_HPP
#define IR_SWITCH_BLOCK_STMT_HPP

#include <memory>
#include <vector>

#include "ir_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRSwitchBlockStmt
     * @brief IRT representation for the switch block
    */
    class IRSwitchBlockStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt switch-block
        */
        IRSwitchBlockStmt();

        /** 
         * @brief getter for the statements of the switch block
         * @returns reference to const vector of pointers to the statements
        */
        const std::vector<std::unique_ptr<IRStmt>>& getStmts() const noexcept;

        /** 
         * @brief adds new statement to switch block
         * @param stmt - pointer to the statement
        */
        void addStmt(std::unique_ptr<IRStmt> stmt);

        /**
         * @brief eliminates statements of the switch block that appear after the node that always returns
         * @param startIdx - index in the vector of statements where deletion starts
        */
        void eliminateDeadStmts(size_t startIdx);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// vector of pointers to statements of the switch-block
        std::vector<std::unique_ptr<IRStmt>> stmts;
        
    };

}

#endif