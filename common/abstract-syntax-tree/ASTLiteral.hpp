#ifndef ASTLITERAL_HPP
#define ASTLITERAL_HPP

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTLiteral
 * @brief AST node representing literal
*/
class ASTLiteral final : public ASTExpression {
public:
    /** 
     * @brief Creates the instance of the ast literal
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the literal
    */
    ASTLiteral(const Token token, ASTNodeType ntype, Types type = Types::NO_TYPE);

};


#endif