#ifndef AST_SWITCH_BLOCK_STMT_HPP
#define AST_SWITCH_BLOCK_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "defs/ast_defs.hpp"
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
     * @param ntype - type of the ast node
    */
    ASTSwitchBlockStmt(const Token& token, ASTNodeType ntype);
    
    /** 
     * @brief getter for the statements of the switch block
     * @returns reference to a vector of the pointers to statements
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement
     * @param statement - pointer to a statement that is being added
    */
    void addStatement(std::unique_ptr<ASTStmt> statement);

    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to statements of the switch-block
    std::vector<std::unique_ptr<ASTStmt>> statements;
};

#endif