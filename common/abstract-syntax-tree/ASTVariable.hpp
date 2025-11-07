#ifndef ASTVARIABLE_HPP
#define ASTVARIABLE_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTVariable
 * @brief AST node representing variable
*/
class ASTVariable final : public ASTStatement {
public:
    ASTVariable(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    /** 
     * @brief getter for the type of the variable
     * @returns type of the variable
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes the type of the variable
     * @param t - type of the variable
     * @returns void
    */
    void setType(Types t) noexcept;

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
     * @returns void
    */
    void setAssign(std::unique_ptr<ASTExpression> assign);

    /** 
     * @brief checks if the variable has assignment
     * @return true if variable has assignment, false otherwise
    */
    bool hasAssign() const noexcept;

    /** 
     * @brief prints variable node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    Types type;
    std::unique_ptr<ASTExpression> assignment;

};

#endif