#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/**
 * @namespace syntax::ast
 * @brief module defining the abstract syntax tree
*/
namespace syntax::ast {
    /** 
     * @class ASTNode
     * @brief parent of AST nodes of all types
    */
    class ASTNode { 
    public:
        /** 
         * @brief Creates the instance of the ast node
         * @param token - const reference to the token
         * @param ntype - type of the ast node
        */
        ASTNode(const syntax::Token& token, ASTNodeType ntype);

        /** 
        * @brief Destructs the instance of the ast node
        */
        virtual ~ASTNode();

        /** 
        * @brief getter for a token of the node
        * @returns const reference to a token describing the node
        */
        const syntax::Token& getToken() const noexcept;

        /** 
        * @brief getter for the node type
        * @returns type of the node
        */
        ASTNodeType getNodeType() const noexcept;

        /**
        * @brief accepts the ast visitor
        * @param visitor - reference to an ast visitor
        */
        virtual void accept(ASTVisitor& visitor) = 0;

    private:
        /// token describing the ast node
        syntax::Token token;
        
        /// type of the ast node
        ASTNodeType nodeType;

    };

}

#endif