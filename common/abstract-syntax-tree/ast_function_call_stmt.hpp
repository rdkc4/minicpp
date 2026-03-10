#ifndef AST_FUNCTION_CALL_STMT_HPP
#define AST_FUNCTION_CALL_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_function_call_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTFunctionCallStmt
 * @brief AST node representing function call statement
*/
class ASTFunctionCallStmt final : public ASTStmt {
public:
    /** 
     * @brief Creates the instance of the ast function call
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the function call
    */
    ASTFunctionCallStmt(const Token& token, ASTNodeType ntype);

    template<typename Self>
    decltype(auto) getFunctionCall(this Self&& self) noexcept {
        return std::forward<Self>(self).functionCall.get();
    }

    /**
     * @brief initializes the function call statement
     * @param funcCall - pointer to a function call expression
    */
    void initFunctionCallSt(std::unique_ptr<ASTFunctionCallExpr> funcCall);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// pointer to the function call expression
    std::unique_ptr<ASTFunctionCallExpr> functionCall;
};

#endif