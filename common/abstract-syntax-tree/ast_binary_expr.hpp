#ifndef AST_BINARY_EXPR_HPP
#define AST_BINARY_EXPR_HPP

#include <memory>

#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"
#include "../defs/types.hpp"
#include "../defs/operators.hpp"

namespace syntax::ast {
    /** 
     * @class ASTBinaryExpr
     * @brief AST node representing binary expression
    */
    class ASTBinaryExpr final : public ASTExpr {
    public:
        /** 
         * @brief Creates the instance of the ast binary expression
         * @param token - const reference to the token
         * @param type - type of the binary expression, defaults to NO_TYPE
         * @param op - operator of the binary expression, defaults to NO_OP
        */
        ASTBinaryExpr(const syntax::Token& token, Type type = Type::NO_TYPE, Operator op = Operator::NO_OP);
        
        /** 
         * @brief getter for left operand expression node
         * @returns pointer or const pointer to left operand expression node
        */
        template<typename Self>
        decltype(auto) getLeftOperandExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).leftOperandExpr.get();
        }

        /** 
         * @brief getter for right operand expression node
         * @returns pointer or const pointer to right operand expression node
        */
        template<typename Self>
        decltype(auto) getRightOperandExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).rightOperandExpr.get();
        }

        /** 
         * @brief getter for the operator of binary expression
         * @returns operator
        */
        Operator getOperator() const noexcept;

        /** 
         * @brief initializes operator
         * @param op - operator of the binary expression
        */
        void setOperator(Operator op) noexcept;

        /**
         * @brief initializes operands
         * @param leftOperand - pointer to a left operand expression node
         * @param rightOperand - pointer to a right operand expression node
        */
        void setOperandExprs(std::unique_ptr<ASTExpr> leftOperand, std::unique_ptr<ASTExpr> rightOperand);

        /** 
         * @brief checks if left and right operands are initialized
         * @returns true if both operands are initialized, otherwise false
        */
        bool initialized() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// pointer to the left operand of the binary expression
        std::unique_ptr<ASTExpr> leftOperandExpr;

        /// pointer to the right operand of the binary expression
        std::unique_ptr<ASTExpr> rightOperandExpr;

        /// operator of the binary expression
        Operator exprOperator;
    };

}

#endif