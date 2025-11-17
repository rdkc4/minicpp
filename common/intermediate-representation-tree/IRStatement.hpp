#ifndef IRSTATEMENT_HPP
#define IRSTATEMENT_HPP

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRStatement
 * @brief IRT representation for all statements
 *
 * parent of all statement classes
*/
class IRStatement : public IRNode {
public:
    IRStatement(IRNodeType ntype);
    
    /** 
     * @brief prints formatted string of the statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override  = 0;
};

#endif