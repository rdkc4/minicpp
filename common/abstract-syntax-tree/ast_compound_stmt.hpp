#ifndef AST_COMPOUND_STMT_HPP
#define AST_COMPOUND_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "defs/ast_defs.hpp"
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
     * @param ntype - type of the ast node
    */
    ASTCompoundStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for statement nodes inside of compound statement
     * @returns reference to a vector of statement nodes
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement node to the vector of statements
     * @param statement - pointer to a statement node
    */
    void addStatement(std::unique_ptr<ASTStmt> statement);

    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to statements of the compound statement
    std::vector<std::unique_ptr<ASTStmt>> statements;

};

#endif