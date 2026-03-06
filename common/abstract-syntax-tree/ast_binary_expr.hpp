#ifndef AST_BINARY_EXPR_HPP
#define AST_BINARY_EXPR_HPP

#include <memory>

#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTBinaryExpr
 * @brief AST node representing binary expression
*/
class ASTBinaryExpr final : public ASTExpr {
public:
    /** 
     * @brief Creates the instance of the ast binary expression
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the binary expression
     * @param op - operator of the binary expression
    */
    ASTBinaryExpr(const Token& token, ASTNodeType ntype, Type type = Type::NO_TYPE, Operator op = Operator::NO_OP);
    
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
    Operator getOperator() const noexcept;

    /** 
     * @brief initializes operator
     * @param _op - operator of the binary expression
    */
    void setOperator(Operator _op) noexcept;

    /**
     * @brief initializes operands
     * @param lOp - pointer to a left operand expression node
     * @param rOp - pointer to a right operand expression node
    */
    void setOperands(std::unique_ptr<ASTExpr> lOp, std::unique_ptr<ASTExpr> rOp);

    /** 
     * @brief checks if left and right operands are initialized
     * @returns true if both operands are initialized, otherwise false
    */
    bool initialized() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the left operand of the binary expression
    std::unique_ptr<ASTExpr> leftOperand;

    /// pointer to the right operand of the binary expression
    std::unique_ptr<ASTExpr> rightOperand;

    /// operator of the binary expression
    Operator op;
};


#endif