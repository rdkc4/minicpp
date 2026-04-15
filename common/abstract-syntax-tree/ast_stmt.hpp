#ifndef AST_STMT_HPP
#define AST_STMT_HPP

#include "ast_node.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTStmt
     * @brief AST node that represents statement, parent node of all statement nodes
    */
    class ASTStmt : public ASTNode {
    public:
        /** 
         * @brief Creates the instance of the ast statement
         * @param token - const reference to the token
         * @param ntype - type of the ast node
        */
        ASTStmt(const Token& token, AST::defs::ASTNodeType ntype);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        virtual void accept(AST::visitor::ASTVisitor& visitor) = 0;

    };

}

#endif