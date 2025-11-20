#ifndef ASTCOMPOUND_ST_HPP
#define ASTCOMPOUND_ST_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTCompoundSt
 * @brief AST node representing compound statement
*/
class ASTCompoundSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast compound statement
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTCompoundSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for statement nodes inside of compound statement
     * @returns reference to a vector of statement nodes
    */
    const std::vector<std::unique_ptr<ASTStatement>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement node to the vector of statements
     * @param statement - pointer to a statement node
     * @returns void
    */
    void addStatement(std::unique_ptr<ASTStatement> statement);

    /** 
     * @brief prints compound statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of pointers to statements of the compound statement
    std::vector<std::unique_ptr<ASTStatement>> statements;

};

#endif