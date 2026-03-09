#ifndef IR_CASE_STMT_HPP
#define IR_CASE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_literal_expr.hpp"
#include "ir_switch_block_stmt.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRCaseStmt
 * @brief IRT representation of the case node
*/
class IRCaseStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt case
     * @param ntype - type of the irt node
    */
    IRCaseStmt(IRNodeType ntype);

    /**
     * @brief getter for the literal of the case
     * @returns pointer or const pointer to the literal
    */
    template<typename Self>
    decltype(auto) getLiteral(this Self&& self) noexcept {
        return std::forward<Self>(self).literal.get();
    }

    /**
     * @brief getter for the switch block of the case statement
     * @returns pointer or const pointer to the switch block node of the case
    */
    template<typename Self>
    decltype(auto) getSwitchBlock(this Self&& self) noexcept {
        return std::forward<Self>(self).swBlock.get();
    }

    /**
     * @brief initializes case statement
     * @param lit - pointer to the literal
     * @param block - pointer to the switch block
     * @param hasBr - flag whether or not case statement has break
    */
    void setCase(std::unique_ptr<IRLiteralExpr> lit, std::unique_ptr<IRSwitchBlockStmt> block, bool hasBr);

    /**
     * @brief getter for the flag for break statement
     * @returns true if case has break statement, false otherwise
    */
    bool hasBreak() const noexcept;

    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the literal of the case
    std::unique_ptr<IRLiteralExpr> literal;

    /// pointer to the switch-block of the case
    std::unique_ptr<IRSwitchBlockStmt> swBlock;

    /// flag if case breaks
    bool breaks;
};


#endif
