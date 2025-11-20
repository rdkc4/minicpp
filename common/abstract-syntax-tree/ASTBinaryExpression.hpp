#ifndef ASTBINARY_EXPRESSION_HPP
#define ASTBINARY_EXPRESSION_HPP

#include <memory>

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTBinaryExpression
 * @brief AST node representing binary expression
*/
class ASTBinaryExpression final : public ASTExpression {
public:
    /** 
     * @brief Creates the instance of the ast binary expression
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the binary expression
     * @param op - operator of the binary expression
    */
    ASTBinaryExpression(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE, Operators op = Operators::NO_OP);
    
    /** 
     * @brief getter for left operand expression node
     * @returns pointer or const pointer to left operand expression node
    */
    template<typename Self>
    decltype(auto) getLeftOperand(this Self&& self) noexcept {
        return std::forward<Self>(self).leftOperand.get();
    }

    /** 
     * @brief getter for right operand expression node
     * @returns pointer or const pointer to right operand expression node
    */
    template<typename Self>
    decltype(auto) getRightOperand(this Self&& self) noexcept {
        return std::forward<Self>(self).rightOperand.get();
    }

    /** 
     * @brief getter for the operator of binary expression
     * @returns operator
    */
    Operators getOperator() const noexcept;

    /** 
     * @brief initializes operator
     * @param _op - operator of the binary expression
     * @returns void
    */
    void setOperator(Operators _op) noexcept;

    /**
     * @brief initializes operands
     * @param lOp - pointer to a left operand expression node
     * @param rOp - pointer to a right operand expression node
     * @returns void 
    */
    void setOperands(std::unique_ptr<ASTExpression> lOp, std::unique_ptr<ASTExpression> rOp);

    /** 
     * @brief checks if left and right operands are initialized
     * @returns true if both operands are initialized, otherwise false
    */
    bool initialized() const noexcept;

    /** 
     * @brief prints binary expression node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// pointer to the left operand of the binary expression
    std::unique_ptr<ASTExpression> leftOperand;
    /// pointer to the right operand of the binary expression
    std::unique_ptr<ASTExpression> rightOperand;
    /// operator of the binary expression
    Operators op;
};


#endif