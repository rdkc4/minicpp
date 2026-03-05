#ifndef ASTDIRECTIVE_HPP
#define ASTDIRECTIVE_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"

/** 
 * @class ASTDirective
 * @brief AST node that represents directive, parent node of all directive nodes
*/
class ASTDirective : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast directive
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTDirective(const Token token, ASTNodeType ntype);

    /** 
     * @brief prints directive node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override = 0;

};

#endif