#ifndef AST_RETURN_STMT_HPP
#define AST_RETURN_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTReturnStmt
 * @brief AST node representing return statement
*/
class ASTReturnStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast return statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTReturnStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for the expression
     * @warning nullable
     * @returns pointer or const pointer to the expression
    */
    template<typename Self>
    decltype(auto) getExp(this Self&& self) noexcept {
        return std::forward<Self>(self).exp.get();
    }

    /** 
     * @brief initializes the expression
     * @param nexp - pointer to the expression
    */
    void setExp(std::unique_ptr<ASTExpr> nexp);

    /** 
     * @brief checks if return statement returns anything
     * @returns false if it returns void, true otherwise
    */
    bool returns() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the expression of the return statement
    std::unique_ptr<ASTExpr> exp;

};

#endif