#ifndef ASTDEFAULT_ST_HPP
#define ASTDEFAULT_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTSwitchBlock.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTDefaultSt
 * @brief AST node representing default statement node
*/
class ASTDefaultSt final : public ASTStatement {
public:
    ASTDefaultSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for switch block node
     * @returns const pointer to a switch block node
    */
    const ASTSwitchBlock* getSwitchBlock() const noexcept;

    /** 
     * @brief initializes switch block node
     * @param _swBlock - pointer to a switch block node
     * @return void
    */
    void setDefault(std::unique_ptr<ASTSwitchBlock> _swBlock);

    /** 
     * @brief prints default statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTSwitchBlock> swBlock;

};

#endif