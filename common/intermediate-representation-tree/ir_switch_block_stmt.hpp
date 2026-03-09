#ifndef IR_SWITCH_BLOCK_STMT_HPP
#define IR_SWITCH_BLOCK_STMT_HPP

#include <memory>
#include <vector>

#include "ir_stmt.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRSwitchBlockStmt
 * @brief IRT representation for the switch block
*/
class IRSwitchBlockStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt switch-block
     * @param ntype - type of the irt node
    */
    IRSwitchBlockStmt(IRNodeType ntype);

    /** 
     * @brief getter for the statements of the switch block
     * @returns reference to const vector of pointers to the statements
    */
    const std::vector<std::unique_ptr<IRStmt>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement to switch block
     * @param statement - pointer to the statement
    */
    void addStatement(std::unique_ptr<IRStmt> statement);

    /**
     * @brief eliminates statements of the switch block that appear after the node that always returns
     * @param startIdx - index in the vector of statements where deletion starts
    */
    void eliminateDead(size_t startIdx);

    void accept(IRVisitor& visitor) override;

private:
    /// vector of pointers to statements of the switch-block
    std::vector<std::unique_ptr<IRStmt>> statements;
    
};

#endif