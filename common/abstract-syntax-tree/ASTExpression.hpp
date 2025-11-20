#ifndef ASTEXPRESSION_HPP
#define ASTEXPRESSION_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTExpression
 * @brief AST node representing expression
 * @note parent class to all expressions
*/
class ASTExpression : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast expression
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the expression
    */
    ASTExpression(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    /** 
     * @brief getter for a type of the expression
     * @returns type of the expression
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes type of the expression node
     * @param t - type of the expression
     * @returns void
    */
    void setType(Types t) noexcept;

    /** 
     * @brief prints expression node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override = 0;

private:
    /// type of the expression
    Types type;
};


#endif