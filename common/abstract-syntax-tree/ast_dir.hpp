#ifndef AST_DIR_HPP
#define AST_DIR_HPP

#include "ast_node.hpp"
#include "defs/ast_defs.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTDir
     * @brief AST node that represents directive, parent node of all directive nodes
    */
    class ASTDir : public ASTNode {
    public:
        /** 
         * @brief Creates the instance of the ast directive
         * @param token - const reference to the token
         * @param ntype - type of the ast node
        */
        ASTDir(const syntax::Token& token, syntax::ast::ASTNodeType ntype);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        virtual void accept(ASTVisitor& visitor) = 0;

    };

}

#endif