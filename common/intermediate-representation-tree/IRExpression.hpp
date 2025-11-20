#ifndef IREXPRESSION_HPP
#define IREXPRESSION_HPP

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRExpression
 * @brief IRT representation for the expression node
 *
 * parent class of all expressions
*/
class IRExpression : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt expression
     * @param ntype - type of the irt node
     * @param type - type of the expression
    */
    IRExpression(IRNodeType ntype, Types type);

    /**
     * @brief getter for the type of the expression
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
     * @brief prints formatted string of the expression node
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
