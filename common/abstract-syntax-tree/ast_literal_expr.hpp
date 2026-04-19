#ifndef AST_LITERAL_EXPR_HPP
#define AST_LITERAL_EXPR_HPP

#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../defs/types.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTLiteralExpr
     * @brief AST node representing literal
    */
    class ASTLiteralExpr final : public ASTExpr {
    public:
        /** 
         * @brief Creates the instance of the ast literal
         * @param token - const reference to the token
         * @param type - type of the literal
        */
        ASTLiteralExpr(const syntax::Token& token, types::Type type = types::Type::NO_TYPE);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    };

}

#endif