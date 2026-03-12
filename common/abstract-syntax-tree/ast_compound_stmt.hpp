#ifndef AST_COMPOUND_STMT_HPP
#define AST_COMPOUND_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTCompoundStmt
 * @brief AST node representing compound statement
*/
class ASTCompoundStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast compound statement
     * @param token - const reference to the token
    */
    ASTCompoundStmt(const Token& token);

    /** 
     * @brief getter for statement nodes inside of compound statement
     * @returns reference to a vector of statement nodes
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getStmts() const noexcept;

    /** 
     * @brief adds new statement node to the vector of statements
     * @param stmt - pointer to a statement node
    */
    void addStmt(std::unique_ptr<ASTStmt> stmt);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to statements of the compound statement
    std::vector<std::unique_ptr<ASTStmt>> stmts;

};

#endif