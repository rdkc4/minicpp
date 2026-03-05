#ifndef ASTNODE_HPP
#define ASTNODE_HPP

#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTNode
 * @brief parent of AST nodes of all types
*/
class ASTNode{ 
public:
    /** 
     * @brief Creates the instance of the ast node
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTNode(const Token token, ASTNodeType ntype);

    /** 
     * @brief Destructs the instance of the ast node
    */
    virtual ~ASTNode();

    /** 
     * @brief getter for a token of the node
     * @returns const reference to a token describing the node
    */
    const Token& getToken() const noexcept;

    /** 
     * @brief getter for the node type
     * @returns type of the node
    */
    ASTNodeType getNodeType() const noexcept;

private:
    /// token describing the ast node
    Token token;
    
    /// type of the ast node
    ASTNodeType nodeType;
};


#endif