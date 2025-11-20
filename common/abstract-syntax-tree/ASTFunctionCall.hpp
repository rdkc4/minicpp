#ifndef ASTFUNCTION_CALL_HPP
#define ASTFUNCTION_CALL_HPP

#include <memory>
#include <vector>

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTFunctionCall
 * @brief AST node representing function call
*/
class ASTFunctionCall final : public ASTExpression {
public:
    /** 
     * @brief Creates the instance of the ast function call
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the function call
    */
    ASTFunctionCall(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    /** 
     * @brief getter for expressions passed to a function as arguments
     * @returns reference to a vector of expressions
    */
    const std::vector<std::unique_ptr<ASTExpression>>& getArguments() const noexcept;

    /** 
     * @brief getter for argument at a specific position
     * @param n - index of the argument
     * @returns const pointer to an expression
    */
    const ASTExpression* getArgumentAtN(size_t n) const noexcept;

    /** 
     * @brief adds new argument
     * @param arg - pointer to the argument node
     * @returns void
    */
    void addArgument(std::unique_ptr<ASTExpression> arg);

    /** 
     * @brief gets the amount of arguments
     * @returns number of the provided arguments
    */
    size_t getArgumentCount() const noexcept;

    /** 
     * @brief prints function call node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of pointers to expressions that represent arguments
    std::vector<std::unique_ptr<ASTExpression>> arguments;
};

#endif