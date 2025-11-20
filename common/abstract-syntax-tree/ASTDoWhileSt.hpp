#ifndef ASTDOWHILE_ST_HPP
#define ASTDOWHILE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTDoWhileSt
 * @brief AST node representing do-while statement
*/
class ASTDoWhileSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast do-while statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTDoWhileSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for relational expression node
     * @returns pointer or const pointer to a relational expression node
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /**
     * @brief getter for statement node
     * @returns const pointer to a statement node 
    */
    const ASTStatement* getStatement() const noexcept;

    /** 
     * @brief initializes do-while statement node
     * @param cond - pointer to a relational expression node
     * @param st - pointer to a statement node
     * @returns void
    */
    void setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st);

    /** 
     * @brief prints do while statement node
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