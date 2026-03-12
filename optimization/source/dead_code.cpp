#include "../dead_code.hpp"

void Optimization::DeadCode::eliminateDeadCode(IRFunction* function){
    size_t currentStmtIdx = 0;
    for(auto& stmt : function->getBody()){
        if(eliminateDeadCode(stmt.get())){
            function->eliminateDeadStmts(currentStmtIdx + 1);
            return;
        }
        ++currentStmtIdx;
    }
}

bool Optimization::DeadCode::eliminateDeadCode(IRStmt* stmt){
    switch(stmt->getNodeType()){
        case IRNodeType::COMPOUND:
            return eliminateDeadCode(static_cast<IRCompoundStmt*>(stmt));
        case IRNodeType::RETURN:
            return eliminateDeadCode(static_cast<IRReturnStmt*>(stmt));
        case IRNodeType::IF:
            return eliminateDeadCode(static_cast<IRIfStmt*>(stmt));
        case IRNodeType::DO_WHILE:
            return eliminateDeadCode(static_cast<IRDoWhileStmt*>(stmt));
        case IRNodeType::SWITCH:
            return eliminateDeadCode(static_cast<IRSwitchStmt*>(stmt));
        default:
            return false;
    }
}

bool Optimization::DeadCode::eliminateDeadCode(IRCompoundStmt* compoundStmt){
    size_t currentStmtIdx = 0;
    for(auto& stmt : compoundStmt->getStmts()){
        if(eliminateDeadCode(stmt.get())){
            compoundStmt->eliminateDeadStmts(currentStmtIdx + 1);
            return true;
        }
        ++currentStmtIdx;
    }
    return false;
}

bool Optimization::DeadCode::eliminateDeadCode([[maybe_unused]] IRReturnStmt* returnStmt){
    return true;
}

bool Optimization::DeadCode::eliminateDeadCode(IRIfStmt* ifStmt){
    bool alwaysReturns = true;
    for(auto& stmt : ifStmt->getStmts()){
        if(!eliminateDeadCode(stmt.get())){
            alwaysReturns = false;
        }
    }
    if(!ifStmt->hasElseStmt()){
        return false;
    }
    return alwaysReturns;
}

bool Optimization::DeadCode::eliminateDeadCode(IRDoWhileStmt* dowhileStmt){
    return eliminateDeadCode(dowhileStmt->getStmt());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchStmt* switchStmt){
    bool returnsAlways = true;
    for(auto& caseStmt : switchStmt->getCaseStmts()){
        if(!eliminateDeadCode(caseStmt.get()) && caseStmt->hasBreakStmt()){
            returnsAlways = false;
        }
    }
    if(!switchStmt->hasDefaultStmt()){
        return false;
    }
    return eliminateDeadCode(switchStmt->getDefaultStmt()) && returnsAlways;
}

bool Optimization::DeadCode::eliminateDeadCode(IRCaseStmt* caseStmt){
    return eliminateDeadCode(caseStmt->getSwitchBlockStmt());
}

bool Optimization::DeadCode::eliminateDeadCode(IRDefaultStmt* defaultStmt){
    return eliminateDeadCode(defaultStmt->getSwitchBlockStmt());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchBlockStmt* switchBlockStmt){
    size_t currentStmtIdx = 0;
    for(auto& stmt : switchBlockStmt->getStmts()){
        if(eliminateDeadCode(stmt.get())){
            switchBlockStmt->eliminateDeadStmts(currentStmtIdx + 1);
            return true;
        }
        ++currentStmtIdx;
    }
    return false;
} 

bool Optimization::DeadCode::eliminateDeadCode([[maybe_unused]] IRNode* node){
    return false;
}