#ifndef ASTPARAMETER_HPP
#define ASTPARAMETER_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTParameter
 * @brief AST node representing parameter
*/
class ASTParameter final : public ASTNode {
public:
    ASTParameter(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);
    
    /** 
     * @brief getter for a type of the parameter
     * @returns type of a parameter
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes type of the parameter
     * @param t - type of a parameter
     * @returns void
    */
    void setType(Types t) noexcept;

    /** 
     * @brief prints parameter node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    Types type;
};


#endif