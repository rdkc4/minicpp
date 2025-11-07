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
    ASTLiteral(const Token token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    /** 
     * @brief prints literal node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

};


#endif