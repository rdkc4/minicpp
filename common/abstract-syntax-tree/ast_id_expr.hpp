#ifndef AST_ID_EXPR_HPP
#define AST_ID_EXPR_HPP

#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../defs/types.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTIdExpr
     * @brief AST node representing id
    */
    class ASTIdExpr final : public ASTExpr {
    public:
        /** 
         * @brief Creates the instance of the ast id
         * @param token - const reference to the token
         * @param type - type of the id
        */
        ASTIdExpr(const syntax::Token& token, Type type = Type::NO_TYPE);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    };

}

#endif