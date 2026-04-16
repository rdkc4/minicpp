#ifndef AST_EXPR_HPP
#define AST_EXPR_HPP

#include "ast_node.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTExpr
     * @brief AST node representing expression
     * @note parent class to all expressions
    */
    class ASTExpr : public ASTNode {
    public:
        /** 
         * @brief Creates the instance of the ast expression
         * @param token - const reference to the token
         * @param ntype - type of the ast node
         * @param type - type of the expression
        */
        ASTExpr(const Token& token, AST::defs::ASTNodeType ntype, Type type = Type::NO_TYPE);

        /** 
         * @brief getter for a type of the expression
         * @returns type of the expression
        */
        Type getType() const noexcept;

        /** 
         * @brief initializes type of the expression node
         * @param exprType - type of the expression
        */
        void setType(Type exprType) noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        virtual void accept(AST::visitor::ASTVisitor& visitor) = 0;

    private:
        /// type of the expression
        Type type;
    };

}

#endif