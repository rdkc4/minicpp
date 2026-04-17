#ifndef AST_FUNCTION_CALL_EXPR_HPP
#define AST_FUNCTION_CALL_EXPR_HPP

#include <memory>
#include <vector>

#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTFunctionCallExpr
     * @brief AST node representing function call
    */
    class ASTFunctionCallExpr final : public ASTExpr {
    public:
        /** 
         * @brief Creates the instance of the ast function call
         * @param token - const reference to the token
         * @param type - type of the function call
        */
        ASTFunctionCallExpr(const syntax::Token& token, Type type = Type::NO_TYPE);

        /** 
         * @brief getter for expressions passed to a function as arguments
         * @returns reference to a vector of expressions
        */
        const std::vector<std::unique_ptr<ASTExpr>>& getArguments() const noexcept;

        /** 
         * @brief getter for argument at a specific position
         * @param n - index of the argument
         * @returns const pointer to an expression
        */
        const ASTExpr* getArgumentAtN(size_t n) const noexcept;

        /** 
         * @brief adds new argument
         * @param expr - pointer to the expression node
        */
        void addArgument(std::unique_ptr<ASTExpr> expr);

        /** 
         * @brief gets the amount of arguments
         * @returns number of the provided arguments
        */
        size_t getArgumentCount() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// vector of pointers to expressions that represent arguments
        std::vector<std::unique_ptr<ASTExpr>> arguments;

    };

}

#endif