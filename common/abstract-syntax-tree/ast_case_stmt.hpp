#ifndef AST_CASE_STMT_HPP
#define AST_CASE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_literal_expr.hpp"
#include "ast_switch_block_stmt.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /**
     * @class ASTCaseStmt
     * @brief AST node representing case statement
    */
    class ASTCaseStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast case
         * @param token - const reference to the token
        */
        ASTCaseStmt(const syntax::Token& token);

        /** 
         * @brief getter for a literal node
         * @returns pointer or const pointer to a literal node
        */
        template<typename Self>
        decltype(auto) getLiteralExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).literalExpr.get();
        }

        /** 
         * @brief getter for switch block node
         * @returns pointer or const pointer to a switch block node
        */
        template<typename Self>
        decltype(auto) getSwitchBlockStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).switchBlockStmt.get();
        }

        /** 
         * @brief initializes case node
         * @param litExpr - pointer to a literal node
         * @param swBlockStmt - pointer to a switch block node
         * @param hasBreak - flag if case has break
        */
        void setCase(
            std::unique_ptr<ASTLiteralExpr> litExpr, 
            std::unique_ptr<ASTSwitchBlockStmt> swBlockStmt, 
            bool hasBreak
        );

        /** 
         * @brief getter for break flag
         * @returns true if case has break, false otherwise
        */
        bool hasBreakStmt() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// pointer to the literal of the case
        std::unique_ptr<ASTLiteralExpr> literalExpr;

        /// pointer to the switch-block of the case
        std::unique_ptr<ASTSwitchBlockStmt> switchBlockStmt;

        /// flag if case breaks
        bool breaks;

    };

}

#endif