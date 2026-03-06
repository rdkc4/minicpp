#ifndef AST_SWITCH_STMT_HPP
#define AST_SWITCH_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "ast_id_expr.hpp"
#include "ast_case_stmt.hpp"
#include "ast_default_stmt.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTSwitchStmt
 * @brief AST node representing switch statement
*/
class ASTSwitchStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast switch-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTSwitchStmt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for a variable (id of a variable)
     * @returns pointer or const pointer to id
    */
    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    /** 
     * @brief initializes id of a variable
     * @param var - pointer to id of a variable
    */
    void setVariable(std::unique_ptr<ASTIdExpr> var);

    /** 
     * @brief getter for the cases of the switch statement
     * @returns reference to a vector of pointers to cases
    */
    const std::vector<std::unique_ptr<ASTCaseStmt>>& getCases() const noexcept;

    /** 
     * @brief getter for the default statement of the switch statement
     * @returns pointer or const pointer to a default statement
    */
    template<typename Self>
    decltype(auto) getDefault(this Self&& self) noexcept {
        return std::forward<Self>(self)._default.get();
    }

    /** 
     * @brief adds new case
     * @param _case - pointer to a case that is being added
    */
    void addCase(std::unique_ptr<ASTCaseStmt> _case);

    /** 
     * @brief adds default statement
     * @param _swDefault - pointer to a default statement
    */
    void setDefault(std::unique_ptr<ASTDefaultStmt> _swDefault);

    /** 
     * @brief checks if default statement exists
     * @returns true if default exists, false otherwise
    */
    bool hasDefault() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to id of the variable of the switch-statement
    std::unique_ptr<ASTIdExpr> variable;

    /// vector of pointer to cases of the switch-statement
    std::vector<std::unique_ptr<ASTCaseStmt>> cases;
    
    /// pointer to the default case of the switch-statement
    std::unique_ptr<ASTDefaultStmt> _default;

};

#endif