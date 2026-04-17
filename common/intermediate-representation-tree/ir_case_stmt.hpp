#ifndef IR_CASE_STMT_HPP
#define IR_CASE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_literal_expr.hpp"
#include "ir_switch_block_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /**
     * @class IRCaseStmt
     * @brief IRT representation of the case node
    */
    class IRCaseStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt case
        */
        IRCaseStmt();

        /**
         * @brief getter for the literal of the case
         * @returns pointer or const pointer to the literal
        */
        template<typename Self>
        decltype(auto) getLiteralExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).literalExpr.get();
        }

        /**
         * @brief getter for the switch block of the case statement
         * @returns pointer or const pointer to the switch block node of the case
        */
        template<typename Self>
        decltype(auto) getSwitchBlockStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).switchBlockStmt.get();
        }

        /**
         * @brief initializes case statement
         * @param litExpr - pointer to the literal
         * @param swBlockStmt - pointer to the switch block
         * @param hasBreak - flag whether or not case statement has break
        */
        void setCase(
            std::unique_ptr<IRLiteralExpr> litExpr, 
            std::unique_ptr<IRSwitchBlockStmt> swBlockStmt, 
            bool hasBreak
        );

        /**
         * @brief getter for the flag for break statement
         * @returns true if case has break statement, false otherwise
        */
        bool hasBreakStmt() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// pointer to the literal of the case
        std::unique_ptr<IRLiteralExpr> literalExpr;

        /// pointer to the switch-block of the case
        std::unique_ptr<IRSwitchBlockStmt> switchBlockStmt;

        /// flag if case breaks
        bool breaks;
    
    };

}

#endif