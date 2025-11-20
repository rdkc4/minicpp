#ifndef ASTIF_ST_HPP
#define ASTIF_ST_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTIfSt
 * @brief AST node representing if statement
*/
class ASTIfSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast if-statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTIfSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for conditions
     * @returns reference to a vector of pointers to relational expression
    */
    const std::vector<std::unique_ptr<ASTExpression>>& getConditions() const noexcept;

    /** 
     * @brief getter for statements
     * @returns reference to a vector of pointers to statement
    */
    const std::vector<std::unique_ptr<ASTStatement>>& getStatements() const noexcept;

    /** 
     * @brief adds new if/else-if statement
     * @param condition - pointer to a relational expression
     * @param statement - pointer to a statement of if/else-if statement
     * @returns void 
    */
    void addIf(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTStatement> statement);

    /** 
     * @brief adds new else statement
     * @param statement - pointer to a statement of else statement
     * @returns void
    */
    void addElse(std::unique_ptr<ASTStatement> statement);

    /** 
     * @brief checks if if-statement ends with else
     * @returns true if else-statement exists, false otherwise
    */
    bool hasElse() const noexcept;

    /** 
     * @brief prints if statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<ASTExpression>> conditions;
    std::vector<std::unique_ptr<ASTStatement>> statements;

};

#endif