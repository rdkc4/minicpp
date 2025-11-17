#ifndef IRSWITCH_BLOCK_HPP
#define IRSWITCH_BLOCK_HPP

#include <memory>
#include <vector>

#include "IRStatement.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRSwitchBlock
 * @brief IRT representation for the switch block
*/
class IRSwitchBlock final : public IRStatement {
public:
    IRSwitchBlock(IRNodeType ntype);

    /** 
     * @brief getter for the statements of the switch block
     * @returns reference to const vector of pointers to the statements
    */
    const std::vector<std::unique_ptr<IRStatement>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement to switch block
     * @param statement - pointer to the statement
     * @returns void
    */
    void addStatement(std::unique_ptr<IRStatement> statement);

    /**
     * @brief eliminates statements of the switch block that appear after the node that always returns
     * @param startIdx - index in the vector of statements where deletion starts
     * @returns void
    */
    void eliminateDead(size_t startIdx);

    /** 
     * @brief prints formatted string of the switch block node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRStatement>> statements;
};

#endif