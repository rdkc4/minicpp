#ifndef IR_NODE_HPP
#define IR_NODE_HPP

#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRNode
 * @brief parent class of all IRT nodes
*/
class IRNode {
public:
    /** 
     * @brief Creates the instance of the irt node
     * @param ntype - type of the irt node
    */
    IRNode(IRNodeType ntype);

    /** 
     * @brief Destructs the instance of the irt node
    */
    virtual ~IRNode();

    /** 
     * @brief getter for the node type
     * @returns type of the node
    */
    IRNodeType getNodeType() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    virtual void accept(IRVisitor& visitor) = 0;

private:
    /// type of the irt node
    IRNodeType nodeType;
};

#endif