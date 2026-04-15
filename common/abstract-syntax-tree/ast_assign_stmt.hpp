#ifndef AST_ASSIGN_STMT_HPP
#define AST_ASSIGN_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_id_expr.hpp"
#include "../token/token.hpp"

namespace AST::node {
    /** 
     * @class ASTAssignStmt
     * @brief AST node representing assignment statement
    */
    class ASTAssignStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast assignment statement
         * @param token - const reference to the token
        */
        ASTAssignStmt(const Token& token);

        /** 
         * @brief getter for variable node
         * @returns pointer or constant pointer to the id node
        */
        template<typename Self>
        decltype(auto) getVariableIdExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).variableIdExpr.get();
        }

        /** 
         * @brief getter for expression node
         * @returns pointer or constant pointer to the expression node
        */
        template<typename Self>
        decltype(auto) getAssignedExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).assignedExpr.get();
        }

        /** 
         * @brief initializes id sub-node
         * @param idExpr - pointer to an id node
        */
        void setVariableIdExpr(std::unique_ptr<ASTIdExpr> idExpr);

        /** 
         * @brief initializes expression sub-node
         * @param expr - pointer to an expression node
        */
        void setAssignedExpr(std::unique_ptr<ASTExpr> expr);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// pointer to the id of the variable
        std::unique_ptr<ASTIdExpr> variableIdExpr;

        /// pointer to the assigned expression
        std::unique_ptr<ASTExpr> assignedExpr;

    };

}

#endif