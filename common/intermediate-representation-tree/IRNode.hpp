#ifndef IRNODE_HPP
#define IRNODE_HPP

#include <string>

#include "defs/IRTree_defs.hpp"

/** 
 * @class IRNode
 * @brief parent class of all IRT nodes
*/
class IRNode {
public:
    IRNode(IRNodeType ntype);
    virtual ~IRNode();

    /** 
     * @brief getter for the node type
     * @returns type of the node
    */
    IRNodeType getNodeType() const noexcept;

    /** 
     * @brief getter for the formatted node type
     * @returns type of the node as string
    */
    std::string toString() const;

    /** 
     * @brief virtual function for formatting the string of the node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    virtual void print(size_t offset) const = 0;

private:
    IRNodeType nodeType;
};

#endif