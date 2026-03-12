#include "../ir_function_call_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRFunctionCallStmt::IRFunctionCallStmt() : IRStmt{ IRNodeType::CALL } {}

void IRFunctionCallStmt::setFunctionCallStmt(std::unique_ptr<IRFunctionCallExpr> callExpr){
    functionCallExpr = std::move(callExpr);
}

void IRFunctionCallStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}