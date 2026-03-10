#ifndef AST_DOWHILE_STMT_HPP
#define AST_DOWHILE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTDoWhileStmt
 * @brief AST node representing do-while statement
*/
class ASTDoWhileStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast do-while statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTDoWhileStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for relational expression node
     * @returns pointer or const pointer to a relational expression node
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /**
     * @brief getter for statement node
     * @returns pointer or const pointer to a statement node 
    */
    template<typename Self>
    decltype(auto) getStatement(this Self&& self) noexcept {
        return std::forward<Self>(self).statement.get();
    }

    /** 
     * @brief initializes do-while statement node
     * @param cond - pointer to a relational expression node
     * @param st - pointer to a statement node
    */
    void setDoWhile(std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTStmt> st);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the relational expression of the do-while statement
    std::unique_ptr<ASTExpr> condition;

    /// pointer to the statement of the do-while statement
    std::unique_ptr<ASTStmt> statement;

};

#endif