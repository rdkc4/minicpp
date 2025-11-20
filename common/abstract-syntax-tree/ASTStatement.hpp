#ifndef ASTSTATEMENT_HPP
#define ASTSTATEMENT_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTStatement
 * @brief AST node that represents statement, parent node of all statement nodes
*/
class ASTStatement : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTStatement(const Token& token, ASTNodeType ntype);

    /** 
     * @brief prints statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override = 0;
};

#endif