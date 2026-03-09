#ifndef IR_STATEMENT_HPP
#define IR_STATEMENT_HPP

#include "ir_node.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRStmt
 * @brief IRT representation for all statements
 *
 * parent of all statement classes
*/
class IRStmt : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt statement
     * @param ntype - type of the irt node
    */
    IRStmt(IRNodeType ntype);
    
    virtual void accept(IRVisitor& visitor) = 0;

};

#endif