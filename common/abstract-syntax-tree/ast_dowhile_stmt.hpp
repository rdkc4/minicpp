#ifndef AST_DOWHILE_STMT_HPP
#define AST_DOWHILE_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
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
    */
    ASTDoWhileStmt(const Token& token);

    /** 
     * @brief getter for condition node
     * @returns pointer or const pointer to a condition node
    */
    template<typename Self>
    decltype(auto) getConditionExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).conditionExpr.get();
    }

    /**
     * @brief getter for statement node
     * @returns pointer or const pointer to a statement node 
    */
    template<typename Self>
    decltype(auto) getStmt(this Self&& self) noexcept {
        return std::forward<Self>(self).stmt.get();
    }

    /** 
     * @brief initializes do-while statement node
     * @param condExpr - pointer to a condition node
     * @param statement - pointer to a statement node
    */
    void setDoWhile(std::unique_ptr<ASTExpr> condExpr, std::unique_ptr<ASTStmt> statement);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the condition of the do-while statement
    std::unique_ptr<ASTExpr> conditionExpr;

    /// pointer to the statement of the do-while statement
    std::unique_ptr<ASTStmt> stmt;

};

#endif