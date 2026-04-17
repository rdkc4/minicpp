#ifndef AST_RETURN_STMT_HPP
#define AST_RETURN_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTReturnStmt
     * @brief AST node representing return statement
    */
    class ASTReturnStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast return statement
         * @param token - const reference to the token
        */
        ASTReturnStmt(const syntax::Token& token);

        /** 
         * @brief getter for the expression
         * @warning nullable
         * @returns pointer or const pointer to the expression
        */
        template<typename Self>
        decltype(auto) getReturnExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).returnExpr.get();
        }

        /** 
         * @brief initializes the expression
         * @param expr - pointer to the expression
        */
        void setReturnExpr(std::unique_ptr<ASTExpr> expr);

        /** 
         * @brief checks if return statement returns anything
         * @returns false if it returns void, true otherwise
        */
        bool hasReturnExpr() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// pointer to the expression of the return statement
        std::unique_ptr<ASTExpr> returnExpr;

    };

}

#endif