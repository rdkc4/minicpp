#ifndef ASTCASE_ST_HPP
#define ASTCASE_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTLiteral.hpp"
#include "ASTSwitchBlock.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/**
 * @class ASTCaseSt
 * @brief AST node representing case statement
*/
class ASTCaseSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast case
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTCaseSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for a literal node
     * @returns const pointer to a literal node
    */
    const ASTLiteral* getLiteral() const noexcept;

    /** 
     * @brief getter for switch block node
     * @returns const pointer to a switch block node
    */
    const ASTSwitchBlock* getSwitchBlock() const noexcept;

    /** 
     * @brief initializes case node
     * @param lit - pointer to a literal node
     * @param _swBlock - pointer to a switch block node
     * @param hasBr - flag if case has break
     * @returns void
    */
    void setCase(std::unique_ptr<ASTLiteral> lit, std::unique_ptr<ASTSwitchBlock> _swBlock, bool hasBr);

    /** 
     * @brief getter for break flag
     * @returns true if breaks, false otherwise
    */
    bool hasBreak() const noexcept;

    /** 
     * @brief prints case statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTLiteral> literal;
    std::unique_ptr<ASTSwitchBlock> swBlock;
    bool _break;

};

#endif