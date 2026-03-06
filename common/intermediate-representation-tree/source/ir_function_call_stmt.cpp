#include "../ir_function_call_stmt.hpp"

IRFunctionCallStmt::IRFunctionCallStmt(IRNodeType ntype) : IRStmt{ ntype } {}

const IRFunctionCallExpr* IRFunctionCallStmt::getFunctionCall() const noexcept {
    return functionCall.get();
}

void IRFunctionCallStmt::setFunctionCallSt(std::unique_ptr<IRFunctionCallExpr> call){
    functionCall = std::move(call);
}