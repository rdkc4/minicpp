#ifndef AST_FUNCTION_CALL_EXPR_HPP
#define AST_FUNCTION_CALL_EXPR_HPP

#include <memory>
#include <vector>

#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTFunctionCallExpr
 * @brief AST node representing function call
*/
class ASTFunctionCallExpr final : public ASTExpr {
public:
    /** 
     * @brief Creates the instance of the ast function call
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the function call
    */
    ASTFunctionCallExpr(const Token& token, ASTNodeType ntype, Type type = Type::NO_TYPE);

    /** 
     * @brief getter for expressions passed to a function as arguments
     * @returns reference to a vector of expressions
    */
    const std::vector<std::unique_ptr<ASTExpr>>& getArguments() const noexcept;

    /** 
     * @brief getter for argument at a specific position
     * @param n - index of the argument
     * @returns const pointer to an expression
    */
    const ASTExpr* getArgumentAtN(size_t n) const noexcept;

    /** 
     * @brief adds new argument
     * @param arg - pointer to the argument node
    */
    void addArgument(std::unique_ptr<ASTExpr> arg);

    /** 
     * @brief gets the amount of arguments
     * @returns number of the provided arguments
    */
    size_t getArgumentCount() const noexcept;

    void accept(ASTVisitor& visitor) override;

private:
    /// vector of pointers to expressions that represent arguments
    std::vector<std::unique_ptr<ASTExpr>> arguments;
};

#endif