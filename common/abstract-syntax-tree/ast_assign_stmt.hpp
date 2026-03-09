#ifndef AST_ASSIGN_STMT_HPP
#define AST_ASSIGN_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_id_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTAssignStmt
 * @brief AST node representing assignment statement
*/
class ASTAssignStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast assignment statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTAssignStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for variable node
     * @returns pointer or constant pointer to the id node
    */
    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    /** 
     * @brief getter for expression node
     * @returns pointer or constant pointer to the expression node
    */
    template<typename Self>
    decltype(auto) getExp(this Self&& self) noexcept {
        return std::forward<Self>(self).exp.get();
    }

    /** 
     * @brief initializes id sub-node
     * @param var - pointer to an id node
    */
    void setVariable(std::unique_ptr<ASTIdExpr> var);

    /** 
     * @brief initializes expression sub-node
     * @param nexp - pointer to an expression node
    */
    void setExp(std::unique_ptr<ASTExpr> nexp);

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the id of the variable
    std::unique_ptr<ASTIdExpr> variable;

    /// pointer to the assigned expression
    std::unique_ptr<ASTExpr> exp;

};


#endif