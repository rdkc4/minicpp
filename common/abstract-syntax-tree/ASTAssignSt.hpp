#ifndef ASTASSIGN_ST_HPP
#define ASTASSIGN_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTId.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTAssignSt
 * @brief AST node representing assignment statement
*/
class ASTAssignSt final : public ASTStatement {
public:
    ASTAssignSt(const Token& token, ASTNodeType ntype);

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
     * @returns void
    */
    void setVariable(std::unique_ptr<ASTId> var);

    /** 
     * @brief initializes expression sub-node
     * @param nexp - pointer to an expression node
     * @returns void
    */
    void setExp(std::unique_ptr<ASTExpression> nexp);

    /** 
     * @brief prints assignment statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTId> variable;
    std::unique_ptr<ASTExpression> exp;

};


#endif