#ifndef ASTFUNCTION_CALL_ST_HPP
#define ASTFUNCTION_CALL_ST_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTFunctionCall.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTFunctionCallSt
 * @brief AST node representing function call statement
*/
class ASTFunctionCallSt final : public ASTStatement {
public:
    /** 
     * @brief Creates the instance of the ast function call
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the function call
    */
    ASTFunctionCallSt(const Token& token, ASTNodeType ntype);

    template<typename Self>
    decltype(auto) getFunctionCall(this Self&& self) noexcept {
        return std::forward<Self>(self).functionCall.get();
    }

    void initFunctionCallSt(std::unique_ptr<ASTFunctionCall> funcCall);

    /** 
     * @brief prints function call statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// pointer to the function call expression
    std::unique_ptr<ASTFunctionCall> functionCall;
};

#endif