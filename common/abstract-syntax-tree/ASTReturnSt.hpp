#ifndef ASTRETURN_ST_HPP
#define ASTRETURN_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTReturnSt
 * @brief AST node representing return statement
*/
class ASTReturnSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast return statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTReturnSt(const Token& token, ASTNodeType ntype);

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
     * @returns void
    */
    void setExp(std::unique_ptr<ASTExpression> nexp);

    /** 
     * @brief checks if return statement returns anything
     * @returns false if it returns void, true otherwise
    */
    bool returns() const noexcept;

    /** 
     * @brief prints return statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> exp;

};

#endif