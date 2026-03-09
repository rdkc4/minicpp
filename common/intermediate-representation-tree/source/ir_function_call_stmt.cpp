#include "../ir_function_call_stmt.hpp"

IRFunctionCallStmt::IRFunctionCallStmt(IRNodeType ntype) : IRStmt{ ntype } {}

void IRFunctionCallStmt::setFunctionCallSt(std::unique_ptr<IRFunctionCallExpr> call){
    functionCall = std::move(call);
}

void IRFunctionCallStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}