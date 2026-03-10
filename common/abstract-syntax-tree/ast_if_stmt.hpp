#ifndef AST_IF_STMT_HPP
#define AST_IF_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTIfStmt
 * @brief AST node representing if statement
*/
class ASTIfStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast if-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTIfStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for conditions
     * @returns reference to a vector of pointers to relational expression
    */
    const std::vector<std::unique_ptr<ASTExpr>>& getConditions() const noexcept;

    /** 
     * @brief getter for statements
     * @returns reference to a vector of pointers to statement
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getStatements() const noexcept;

    /** 
     * @brief adds new if/else-if statement
     * @param condition - pointer to a relational expression
     * @param statement - pointer to a statement of if/else-if statement
    */
    void addIf(std::unique_ptr<ASTExpr> condition, std::unique_ptr<ASTStmt> statement);

    /** 
     * @brief adds new else statement
     * @param statement - pointer to a statement of else statement
    */
    void addElse(std::unique_ptr<ASTStmt> statement);

    /** 
     * @brief checks if if-statement ends with else
     * @returns true if else-statement exists, false otherwise
    */
    bool hasElse() const noexcept;

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to relational expressions of the if-statement
    std::vector<std::unique_ptr<ASTExpr>> conditions;
    
    /// vector of pointers to statements of the if-statement
    std::vector<std::unique_ptr<ASTStmt>> statements;

};

#endif