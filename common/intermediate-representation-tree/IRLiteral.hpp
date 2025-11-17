#ifndef IRLITERAL_HPP
#define IRLITERAL_HPP

#include <string>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/** 
 * @class IRLiteral
 * @brief IRT representation of the literal node
*/
class IRLiteral final : public IRExpression {
public:
    IRLiteral(IRNodeType ntype, const std::string& val, Types type);

    /** 
     * @brief getter for the value of the literal
     * @returns reference to a value of the literal as const string
    */
    const std::string& getValue() const noexcept;

    /** 
     * @brief initializes the value of the literal
     * @param val - reference to a value
     * @returns void
    */
    void setValue(const std::string& val);

    /** 
     * @brief prints formatted string of the literal node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::string value;
};

#endif