#ifndef AST_DEFAULT_STMT_HPP
#define AST_DEFAULT_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_switch_block_stmt.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTDefaultStmt
     * @brief AST node representing default statement node
    */
    class ASTDefaultStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast default case
         * @param token - const reference to the token
        */
        ASTDefaultStmt(const syntax::Token& token);

        /** 
         * @brief getter for switch block node
         * @returns pointer or const pointer to a switch block node
        */
        template<typename Self>
        decltype(auto) getSwitchBlockStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).switchBlockStmt.get();
        }

        /** 
         * @brief initializes switch block node
         * @param swBlockStmt - pointer to a switch block node
        */
        void setDefaultStmt(std::unique_ptr<ASTSwitchBlockStmt> swBlockStmt);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// pointer to the switch-block of the default case
        std::unique_ptr<ASTSwitchBlockStmt> switchBlockStmt;

    };

}

#endif