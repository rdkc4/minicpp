#ifndef AST_DEFAULT_STMT_HPP
#define AST_DEFAULT_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_switch_block_stmt.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTDefaultStmt
 * @brief AST node representing default statement node
*/
class ASTDefaultStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast default case
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTDefaultStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for switch block node
     * @returns pointer or const pointer to a switch block node
    */
    template<typename Self>
    decltype(auto) getSwitchBlock(this Self&& self) noexcept {
        return std::forward<Self>(self).swBlock.get();
    }

    /** 
     * @brief initializes switch block node
     * @param _swBlock - pointer to a switch block node
     * @return void
    */
    void setDefault(std::unique_ptr<ASTSwitchBlockStmt> _swBlock);

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the switch-block of the default case
    std::unique_ptr<ASTSwitchBlockStmt> swBlock;

};

#endif