#ifndef ASTWHILE_ST_HPP
#define ASTWHILE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTWhileSt
 * @brief AST node representing while statement
*/
class ASTWhileSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast while-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTWhileSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for the condition of the while statement
     * @returns pointer or const pointer to the relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /** 
     * @brief getter for the statement of the while statement
     * @returns const pointer to a statement
    */
    const ASTStatement* getStatement() const noexcept;

    /** 
     * @brief initializes while node
     * @param cond - pointer to the relational expression
     * @param st - pointer to the statement
     * @returns void
    */
    void setWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st);

    /** 
     * @brief prints while statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTExpression> condition;
    std::unique_ptr<ASTStatement> statement;

};

#endif