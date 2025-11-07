#ifndef ASTPRINTF_ST_HPP
#define ASTPRINTF_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/**
 * @class ASTPrintfSt
 * @brief AST node representing printf statement
*/
class ASTPrintfSt final : public ASTStatement {
public:
    ASTPrintfSt(const Token& token, ASTNodeType ntype);
    
    /** 
     * @brief getter for the expression
     * @returns pointer or const pointer to the expression of printf statement
    */
    template<typename Self>
    decltype(auto) getExp(this Self&& self) noexcept {
        return std::forward<Self>(self).exp.get();
    }

    /** 
     * @brief initializes the expression node
     * @param nexp - pointer to expression node
     * @returns void
    */
    void setExp(std::unique_ptr<ASTExpression> nexp);

    /** 
     * @brief prints printf statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> exp;

};


#endif