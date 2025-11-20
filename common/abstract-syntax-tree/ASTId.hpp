#ifndef ASTID_HPP
#define ASTID_HPP

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTId
 * @brief AST node representing id
*/
class ASTId final : public ASTExpression {
public:
    /** 
     * @brief Creates the instance of the ast id
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the id
    */
    ASTId(const Token token, ASTNodeType ntype, Types type = Types::NO_TYPE);
    
    /** 
     * @brief prints id node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;
};


#endif