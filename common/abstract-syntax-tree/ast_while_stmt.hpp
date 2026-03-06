#ifndef AST_WHILE_STMT_HPP
#define AST_WHILE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTWhileStmt
 * @brief AST node representing while statement
*/
class ASTWhileStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast while-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTWhileStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for the condition of the while statement
     * @returns pointer or const pointer to the relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /** 
     * @brief getter for the statement of the while statement
     * @returns const pointer to a statement
    */
    const ASTStmt* getStatement() const noexcept;

    /** 
     * @brief initializes while node
     * @param cond - pointer to the relational expression
     * @param st - pointer to the statement
    */
    void setWhile(std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTStmt> st);

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the relational expression of the while-statement
    std::unique_ptr<ASTExpr> condition;
    
    /// pointer to the statement of the while-statement
    std::unique_ptr<ASTStmt> statement;

};

#endif