#include "../ir_function_call_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRFunctionCallStmt::IRFunctionCallStmt() 
    : IRStmt{ ir::IRNodeType::CALL_STMT } {}

void ir::IRFunctionCallStmt::setFunctionCallStmt(std::unique_ptr<IRFunctionCallExpr> callExpr){
    functionCallExpr = std::move(callExpr);
}

void ir::IRFunctionCallStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}