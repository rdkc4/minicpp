#ifndef AST_FUNCTION_CALL_STMT_HPP
#define AST_FUNCTION_CALL_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_function_call_expr.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTFunctionCallStmt
     * @brief AST node representing function call statement
    */
    class ASTFunctionCallStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast function call statement
         * @param token - const reference to the token
        */
        ASTFunctionCallStmt(const Token& token);

        /**
         * @brief getter for the function call expression
         * @returns pointer or const pointer to function call expression
        */
        template<typename Self>
        decltype(auto) getFunctionCallExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).functionCallExpr.get();
        }

        /**
         * @brief initializes the function call statement
         * @param callExpr - pointer to a function call expression
        */
        void setFunctionCallStmt(std::unique_ptr<ASTFunctionCallExpr> callExpr);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// pointer to the function call expression
        std::unique_ptr<ASTFunctionCallExpr> functionCallExpr;
        
    };

}

#endif