#include "../stack_memory.hpp"

size_t Optimization::StackMemory::computeStackMemory(const IRFunction* function){
    size_t variableCount = 0;
    for(const auto& stmt : function->getBody()){
        variableCount += computeStackMemory(stmt.get());
    }
    return variableCount * regSize;
}

size_t Optimization::StackMemory::computeStackMemory(const IRTemporaryExpr* temporaryExpr){
    size_t variableCount = 0;
    for(const auto& expr : temporaryExpr->getTemporaryExprs()){
        ++variableCount;
        if(expr->getNodeType() == IRNodeType::CALL){
            variableCount += computeStackMemory(expr.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRStmt* stmt){
    switch(stmt->getNodeType()){
        case IRNodeType::VARIABLE:
            return computeStackMemory(static_cast<const IRVariableDeclStmt*>(stmt));
        case IRNodeType::COMPOUND:
            return computeStackMemory(static_cast<const IRCompoundStmt*>(stmt));
        case IRNodeType::IF:
            return computeStackMemory(static_cast<const IRIfStmt*>(stmt));
        case IRNodeType::FOR:
            return computeStackMemory(static_cast<const IRForStmt*>(stmt));
        case IRNodeType::WHILE:
            return computeStackMemory(static_cast<const IRWhileStmt*>(stmt));
        case IRNodeType::DO_WHILE:
            return computeStackMemory(static_cast<const IRDoWhileStmt*>(stmt));
        case IRNodeType::ASSIGN:
            return computeStackMemory(static_cast<const IRAssignStmt*>(stmt));
        case IRNodeType::RETURN:
            return computeStackMemory(static_cast<const IRReturnStmt*>(stmt));
        case IRNodeType::SWITCH:
            return computeStackMemory(static_cast<const IRSwitchStmt*>(stmt));
        case IRNodeType::CALL:
            return computeStackMemory(static_cast<const IRFunctionCallStmt*>(stmt));
        default:
            return 0;
    }
}

size_t Optimization::StackMemory::computeStackMemory(const IRVariableDeclStmt* variableDecl){
    size_t variableCount = 1;
    if(variableDecl->hasTemporaryExpr()){
        variableCount += computeStackMemory(variableDecl->getTemporaryExpr());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRCompoundStmt* compoundStmt){
    size_t variableCount = 0;
    for(const auto& stmt : compoundStmt->getStmts()){
        variableCount += computeStackMemory(stmt.get());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRIfStmt* ifStmt){
    size_t variableCount = 0;
    for(const auto& stmt : ifStmt->getStmts()){
        variableCount += computeStackMemory(stmt.get());
    }
    for(const auto& tempExpr : ifStmt->getTemporaryExprs()){
        if(tempExpr){
            variableCount += computeStackMemory(tempExpr.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRForStmt* forStmt){
    size_t variableCount = computeStackMemory(forStmt->getStmt());
    if(forStmt->hasTemporaryExpr()){
        variableCount += computeStackMemory(forStmt->getTemporaryExpr());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRWhileStmt* whileStmt){
    size_t variableCount = computeStackMemory(whileStmt->getStmt());
    if(whileStmt->hasTemporaryExpr()){
        variableCount += computeStackMemory(whileStmt->getTemporaryExpr());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRDoWhileStmt* dowhileStmt){
    size_t variableCount = computeStackMemory(dowhileStmt->getStmt());
    if(dowhileStmt->hasTemporaryExpr()){
        variableCount += computeStackMemory(dowhileStmt->getTemporaryExpr());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRAssignStmt* assignStmt){
    if(assignStmt->hasTemporaryExpr()){
        return computeStackMemory(assignStmt->getTemporaryExpr());
    }
    return 0;
}

size_t Optimization::StackMemory::computeStackMemory(const IRReturnStmt* returnStmt){
    if(returnStmt->hasTemporaryExpr()){
        return computeStackMemory(returnStmt->getTemporaryExpr());
    }
    return 0;
}

size_t Optimization::StackMemory::computeStackMemory(const IRSwitchStmt* switchStmt){
    size_t variableCount = 0;
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        variableCount += computeStackMemory(caseStmt->getSwitchBlockStmt());
    }
    if(switchStmt->hasDefaultStmt()){
        variableCount += computeStackMemory(switchStmt->getDefaultStmt()->getSwitchBlockStmt());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRSwitchBlockStmt* switchBlockStmt){
    size_t variableCount = 0;
    for(const auto& stmt : switchBlockStmt->getStmts()){
        variableCount += computeStackMemory(stmt.get());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRFunctionCallStmt* callStmt){
    return computeStackMemory(callStmt->getFunctionCallExpr());
}

size_t Optimization::StackMemory::computeStackMemory(const IRFunctionCallExpr* callExpr){
    size_t variableCount = 0;
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr){
            variableCount += computeStackMemory(tempExpr.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory([[maybe_unused]] const IRNode* node){
    return 0;
}