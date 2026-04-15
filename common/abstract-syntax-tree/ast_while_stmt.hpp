#ifndef AST_WHILE_STMT_HPP
#define AST_WHILE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTWhileStmt
     * @brief AST node representing while statement
    */
    class ASTWhileStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast while-statement
         * @param token - const reference to the token
        */
        ASTWhileStmt(const Token& token);

        /** 
         * @brief getter for the condition of the while statement
         * @returns pointer or const pointer to the conditions of the while statement
        */
        template<typename Self>
        decltype(auto) getConditionExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).conditionExpr.get();
        }

        /** 
         * @brief getter for the statement of the while statement
         * @returns pointer or const pointer to a statement
        */
        template<typename Self>
        decltype(auto) getStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).stmt.get();
        }

        /** 
         * @brief initializes while node
         * @param condExpr - pointer to the conditions of the while statement
         * @param statement - pointer to the statement
        */
        void setWhileStmt(std::unique_ptr<ASTExpr> condExpr, std::unique_ptr<ASTStmt> statement);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// pointer to the condition of the while-statement
        std::unique_ptr<ASTExpr> conditionExpr;
        
        /// pointer to the statement of the while-statement
        std::unique_ptr<ASTStmt> stmt;

    };

}

#endif