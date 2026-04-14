#ifndef IR_BINARY_EXPR_HPP
#define IR_BINARY_EXPR_HPP

#include <memory>

#include "ir_expr.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

namespace IR::node {
    /**
     * @class IRBinaryExpr
     * @brief IRT representation of the binary expression node
    */
    class IRBinaryExpr final : public IRExpr {
    public:
        /** 
         * @brief Creates the instance of the irt binary expression
         * @param ntype - type of the irt node
         * @param type - type of the binary expression
        */
        IRBinaryExpr(IR::defs::IRNodeType ntype, Type type);

        /**
         * @brief getter for the left operand of the binary expression
         * @returns pointer or const pointer to the expression node
        */
        template<typename Self>
        decltype(auto) getLeftOperandExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).leftOperandExpr.get();
        }

        /**
         * @brief getter for the right operand of the binary expression 
         * @returns pointer or const pointer to the expression node
        */
        template<typename Self>
        decltype(auto) getRightOperandExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).rightOperandExpr.get();
        }

        /**
         * @brief getter for the operator of the binary expression
         * @returns operator of the binary expression
        */
        Operator getOperator() const noexcept;

        /**
         * @brief initializes the binary expression node
         * @param leftOperand - pointer to the left operand
         * @param rightOperand - pointer to the right operand
         * @param op - operator of the binary expression
        */
        void setBinaryExpr(std::unique_ptr<IRExpr> leftOperand, std::unique_ptr<IRExpr> rightOperand, Operator op);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IR::visitor::IRVisitor& visitor) override;

    private:
        /// pointer to the left operand of the binary expression
        std::unique_ptr<IRExpr> leftOperandExpr;

        /// pointer to the right operand of the binary expression
        std::unique_ptr<IRExpr> rightOperandExpr;
        
        /// operator of the binary expression
        Operator exprOperator;

    };

}

#endif