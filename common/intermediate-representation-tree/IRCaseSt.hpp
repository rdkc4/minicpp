#ifndef IRCASE_ST_HPP
#define IRCASE_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRLiteral.hpp"
#include "IRSwitchBlock.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRCaseSt
 * @brief IRT representation of the case node
*/
class IRCaseSt final : public IRStatement {
public:
    IRCaseSt(IRNodeType ntype);

    /**
     * @brief getter for the literal of the case
     * @returns const pointer to the literal
    */
    const IRLiteral* getLiteral() const noexcept;

    /**
     * @brief getter for the switch block of the case statement
     * @returns pointer or const pointer to the switch block node of the case
    */
    template<typename Self>
    decltype(auto) getSwitchBlock(this Self&& self) noexcept {
        return std::forward<Self>(self).swBlock.get();
    }

    /**
     * @brief initializes case statement
     * @param lit - pointer to the literal
     * @param block - pointer to the switch block
     * @param hasBr - flag whether or not case statement has break
     * @returns void
    */
    void setCase(std::unique_ptr<IRLiteral> lit, std::unique_ptr<IRSwitchBlock> block, bool hasBr);

    /**
     * @brief getter for the flag for break statement
     * @returns true if case has break statement, false otherwise
    */
    bool hasBreak() const noexcept;

    /**
     * @brief prints formatted string of the case statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRLiteral> literal;
    std::unique_ptr<IRSwitchBlock> swBlock;
    bool breaks;
};


#endif
