#ifndef IR_NODE_HPP
#define IR_NODE_HPP

#include <string>

#include "defs/ir_defs.hpp"

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

private:
    /// type of the irt node
    IRNodeType nodeType;
};

#endif