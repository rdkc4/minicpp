#ifndef IR_EXPR_HPP
#define IR_EXPR_HPP

#include "ir_node.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

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
    IRExpr(IRNodeType ntype, Type type);

    /**
     * @brief getter for the type of the expression
     * @returns type of the expression
    */
    Type getType() const noexcept;

    /**
     * @brief initializes type of the expression node
     * @param t - type of the expression
    */
    void setType(Type t) noexcept;

    virtual void accept(IRVisitor& visitor) = 0;

private:
    /// type of the expression
    Type type;
};

#endif
