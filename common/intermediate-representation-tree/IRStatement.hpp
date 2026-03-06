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
    /** 
     * @brief Creates the instance of the irt statement
     * @param ntype - type of the irt node
    */
    IRStatement(IRNodeType ntype);
    
};

#endif