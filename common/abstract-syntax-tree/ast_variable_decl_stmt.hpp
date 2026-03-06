#ifndef AST_VARIABLE_DECL_STMT_HPP
#define AST_VARIABLE_DECL_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTVariableDeclStmt
 * @brief AST node representing variable
*/
class ASTVariableDeclStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast variable declaration
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the variable
    */
    ASTVariableDeclStmt(const Token& token, ASTNodeType ntype, Type type = Type::NO_TYPE);

    /** 
     * @brief getter for the type of the variable
     * @returns type of the variable
    */
    Type getType() const noexcept;

    /** 
     * @brief initializes the type of the variable
     * @param t - type of the variable
    */
    void setType(Type t) noexcept;

    /** 
     * @brief getter for the expression assigned to the variable
     * @warning nullable
     * @returns pointer or const pointer to the expression assigned to the variable
    */
    template<typename Self>
    decltype(auto) getAssign(this Self&& self) noexcept {
        return std::forward<Self>(self).assignment.get();
    }

    /** 
     * @brief initializes expression of the variable
     * @param assign - expression being assigned to the variable
    */
    void setAssign(std::unique_ptr<ASTExpr> assign);

    /** 
     * @brief checks if the variable has assignment
     * @return true if variable has assignment, false otherwise
    */
    bool hasAssign() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// type of the variable
    Type type;
    
    /// pointer to the expression assigned to the variable
    std::unique_ptr<ASTExpr> assignment;

};

#endif