#ifndef IRFUNCTION_CALL_ST_HPP
#define IRFUNCTION_CALL_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRFunctionCall.hpp"

class IRFunctionCallSt : public IRStatement {
public:
    /** 
     * @brief Creates the instance of the irt function-call statement
     * @param ntype - type of the irt node
    */
    IRFunctionCallSt(IRNodeType ntype);

    const IRFunctionCall* getFunctionCall() const noexcept;

    void setFunctionCallSt(std::unique_ptr<IRFunctionCall> call);

    /** 
     * @brief prints formatted string of the function-call statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRFunctionCall> functionCall; 
};

#endif