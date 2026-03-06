#ifndef AST_CASE_STMT_HPP
#define AST_CASE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_literal_expr.hpp"
#include "ast_switch_block_stmt.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/**
 * @class ASTCaseStmt
 * @brief AST node representing case statement
*/
class ASTCaseStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast case
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTCaseStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for a literal node
     * @returns const pointer to a literal node
    */
    const ASTLiteralExpr* getLiteral() const noexcept;

    /** 
     * @brief getter for switch block node
     * @returns const pointer to a switch block node
    */
    const ASTSwitchBlockStmt* getSwitchBlock() const noexcept;

    /** 
     * @brief initializes case node
     * @param lit - pointer to a literal node
     * @param _swBlock - pointer to a switch block node
     * @param hasBr - flag if case has break
    */
    void setCase(std::unique_ptr<ASTLiteralExpr> lit, std::unique_ptr<ASTSwitchBlockStmt> _swBlock, bool hasBr);

    /** 
     * @brief getter for break flag
     * @returns true if breaks, false otherwise
    */
    bool hasBreak() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the literal of the case
    std::unique_ptr<ASTLiteralExpr> literal;

    /// pointer to the switch-block of the case
    std::unique_ptr<ASTSwitchBlockStmt> swBlock;

    /// flag if case breaks
    bool _break;

};

#endif