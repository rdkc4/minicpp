#include "../ir_function_call_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRFunctionCallStmt::IRFunctionCallStmt() 
    : IRStmt{ IR::defs::IRNodeType::CALL_STMT } {}

void IR::node::IRFunctionCallStmt::setFunctionCallStmt(std::unique_ptr<IRFunctionCallExpr> callExpr){
    functionCallExpr = std::move(callExpr);
}

void IR::node::IRFunctionCallStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}