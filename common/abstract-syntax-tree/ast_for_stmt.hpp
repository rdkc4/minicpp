#ifndef AST_FOR_STMT_HPP
#define AST_FOR_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_assign_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTForStmt
 * @brief AST node representing for statement
*/
class ASTForStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast for-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTForStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for initializer
     * @warning nullable
     * @returns const pointer to an assignment statement
    */
    const ASTAssignStmt* getInitializer() const noexcept;

    /** 
     * @brief getter for condition 
     * @warning nullable
     * @returns pointer or const pointer to relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /** 
     * @brief getter for incrementer
     * @warning nullable
     * @returns pointer or const pointer to assignment statement
    */
    const ASTAssignStmt* getIncrementer() const noexcept;
    
    /** 
     * @brief getter for statement
     * @returns const pointer to a statement
    */
    const ASTStmt* getStatement() const noexcept;

    /** 
     * @brief initializes for statement node
     * @param init - pointer to an assignment statement (initializer) 
     * @param cond - pointer to a relational expression (condition)
     * @param st - pointer to a statement node
    */
    void setForSt(std::unique_ptr<ASTAssignStmt> init, std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTAssignStmt> inc, std::unique_ptr<ASTStmt> st);

    /** 
     * @brief checks if initializer is initialized
     * @returns true if initializer is not null, otherwise false
    */
    bool hasInitializer() const noexcept;

    /** 
     * @brief checks if condition is initialized
     * @returns true if condition is not null, otherwise false
    */
    bool hasCondition() const noexcept;

    /** 
     * @brief checks if incrementer is initialized
     * @returns true if incrementer is not null, otherwise false
    */
    bool hasIncrementer() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the assignment statement of the initializer
    std::unique_ptr<ASTAssignStmt> initializer;

    /// pointer to the relational expression of the for-statement
    std::unique_ptr<ASTExpr> condition;

    /// pointer to the assignment statement of the incrementer
    std::unique_ptr<ASTAssignStmt> incrementer;

    /// pointer to the statement of the for-statement
    std::unique_ptr<ASTStmt> statement;

};

#endif