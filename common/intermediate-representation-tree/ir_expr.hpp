#ifndef IR_EXPR_HPP
#define IR_EXPR_HPP

#include "ir_node.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/types.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /**
     * @class IRExpr
     * @brief IRT representation for the expression node
     *
     * parent class of all expressions
    */
    class IRExpr : public IRNode {
    public:
        /** 
         * @brief Creates the instance of the irt expression
         * @param ntype - type of the irt node
         * @param type - type of the expression
        */
        IRExpr(IRNodeType ntype, types::Type type);

        /**
        * @brief getter for the type of the expression
        * @returns type of the expression
        */
        types::Type getType() const noexcept;

        /**
        * @brief initializes type of the expression node
        * @param exprType - type of the expression
        */
        void setType(types::Type exprType) noexcept;

        /**
        * @brief accepts the ir visitor
        * @param visitor - reference to an ir visitor
        */
        virtual void accept(IRVisitor& visitor) = 0;

    private:
        /// type of the expression
        types::Type type;
    };

}

#endif
