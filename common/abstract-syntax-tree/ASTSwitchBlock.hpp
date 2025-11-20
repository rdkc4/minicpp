#ifndef ASTSWITCH_BLOCK_HPP
#define ASTSWITCH_BLOCK_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTSwitchBlock
 * @brief AST node representing switch block
*/
class ASTSwitchBlock final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast switch-block
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTSwitchBlock(const Token& token, ASTNodeType ntype);
    
    /** 
     * @brief getter for the statements of the switch block
     * @returns reference to a vector of the pointers to statements
    */
    const std::vector<std::unique_ptr<ASTStatement>>& getStatements() const noexcept;

    /** 
     * @brief adds new statement
     * @param statement - pointer to a statement that is being added
     * @returns void
    */
    void addStatement(std::unique_ptr<ASTStatement> statement);

    /** 
     * @brief prints switch block node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of pointers to statements of the switch-block
    std::vector<std::unique_ptr<ASTStatement>> statements;
};

#endif