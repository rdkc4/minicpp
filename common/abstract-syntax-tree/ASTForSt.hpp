#ifndef ASTFOR_ST_HPP
#define ASTFOR_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTAssignSt.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTForSt
 * @brief AST node representing for statement
*/
class ASTForSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast for-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTForSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for initializer
     * @warning nullable
     * @returns const pointer to an assignment statement
    */
    const ASTAssignSt* getInitializer() const noexcept;

    /** 
     * @brief getter for condition 
     * @warning nullable
     * @returns pointer or const pointer to relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /** 
     * @brief getter for incrementer
     * @warning nullable
     * @returns pointer or const pointer to assignment statement
    */
    const ASTAssignSt* getIncrementer() const noexcept;
    
    /** 
     * @brief getter for statement
     * @returns const pointer to a statement
    */
    const ASTStatement* getStatement() const noexcept;

    /** 
     * @brief initializes for statement node
     * @param init - pointer to an assignment statement (initializer) 
     * @param cond - pointer to a relational expression (condition)
     * @param st - pointer to a statement node
    */
    void setForSt(std::unique_ptr<ASTAssignSt> init, std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTAssignSt> inc, std::unique_ptr<ASTStatement> st);

    /** 
     * @brief checks if initializer is initialized
     * @returns true if initializer is not null, otherwise false
    */
    bool hasInitializer() const noexcept;

    /** 
     * @brief checks if condition is initialized
     * @returns true if condition is not null, otherwise false
    */
    bool hasCondition() const noexcept;

    /** 
     * @brief checks if incrementer is initialized
     * @returns true if incrementer is not null, otherwise false
    */
    bool hasIncrementer() const noexcept;

    /** 
     * @brief prints for statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// pointer to the assignment statement of the initializer
    std::unique_ptr<ASTAssignSt> initializer;
    /// pointer to the relational expression of the for-statement
    std::unique_ptr<ASTExpression> condition;
    /// pointer to the assignment statement of the incrementer
    std::unique_ptr<ASTAssignSt> incrementer;
    /// pointer to the statement of the for-statement
    std::unique_ptr<ASTStatement> statement;

};

#endif