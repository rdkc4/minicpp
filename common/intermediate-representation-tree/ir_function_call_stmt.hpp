#ifndef IR_FUNCTION_CALL_STMT_HPP
#define IR_FUNCTION_CALL_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_function_call_expr.hpp"

class IRFunctionCallStmt : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt function-call statement
     * @param ntype - type of the irt node
    */
    IRFunctionCallStmt(IRNodeType ntype);

    const IRFunctionCallExpr* getFunctionCall() const noexcept;

    void setFunctionCallSt(std::unique_ptr<IRFunctionCallExpr> call);

private:
    std::unique_ptr<IRFunctionCallExpr> functionCall; 
};

#endif