#ifndef AST_SWITCH_BLOCK_STMT_HPP
#define AST_SWITCH_BLOCK_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTSwitchBlockStmt
 * @brief AST node representing switch block
*/
class ASTSwitchBlockStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast switch-block
     * @param token - const reference to the token
    */
    ASTSwitchBlockStmt(const Token& token);
    
    /** 
     * @brief getter for the statements of the switch block
     * @returns reference to a vector of the pointers to statements
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getStmts() const noexcept;

    /** 
     * @brief adds new statement
     * @param stmt - pointer to a statement that is being added
    */
    void addStmt(std::unique_ptr<ASTStmt> stmt);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to statements of the switch-block
    std::vector<std::unique_ptr<ASTStmt>> stmts;
};

#endif