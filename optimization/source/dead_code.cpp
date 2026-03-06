#include "../dead_code.hpp"

void Optimization::DeadCode::eliminateDeadCode(IRFunction* _function){
    size_t currentStmtIdx = 0;
    for(auto& stmt : _function->getBody()){
        if(eliminateDeadCode(stmt.get())){
            _function->eliminateDead(currentStmtIdx + 1);
            return;
        }
        ++currentStmtIdx;
    }
}

bool Optimization::DeadCode::eliminateDeadCode(IRStmt* _stmt){
    switch(_stmt->getNodeType()){
        case IRNodeType::COMPOUND:
            return eliminateDeadCode(static_cast<IRCompoundStmt*>(_stmt));
        case IRNodeType::RETURN:
            return eliminateDeadCode(static_cast<IRReturnStmt*>(_stmt));
        case IRNodeType::IF:
            return eliminateDeadCode(static_cast<IRIfStmt*>(_stmt));
        case IRNodeType::DO_WHILE:
            return eliminateDeadCode(static_cast<IRDoWhileStmt*>(_stmt));
        case IRNodeType::SWITCH:
            return eliminateDeadCode(static_cast<IRSwitchStmt*>(_stmt));
        default:
            return false;
    }
}

bool Optimization::DeadCode::eliminateDeadCode(IRCompoundStmt* _compound){
    size_t currentStmtIdx = 0;
    for(auto& stmt : _compound->getStatements()){
        if(eliminateDeadCode(stmt.get())){
            _compound->eliminateDead(currentStmtIdx + 1);
            return true;
        }
        ++currentStmtIdx;
    }
    return false;
}

bool Optimization::DeadCode::eliminateDeadCode([[maybe_unused]] IRReturnStmt* _return){
    return true;
}

bool Optimization::DeadCode::eliminateDeadCode(IRIfStmt* _if){
    bool alwaysReturns = true;
    for(auto& _statement : _if->getStatements()){
        if(!eliminateDeadCode(_statement.get())){
            alwaysReturns = false;
        }
    }
    if(!_if->hasElse()){
        return false;
    }
    return alwaysReturns;
}

bool Optimization::DeadCode::eliminateDeadCode(IRDoWhileStmt* _dowhile){
    return eliminateDeadCode(_dowhile->getStatement());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchStmt* _switch){
    bool returnsAlways = true;
    for(auto& _case : _switch->getCases()){
        if(!eliminateDeadCode(_case.get()) && _case->hasBreak()){
            returnsAlways = false;
        }
    }
    if(!_switch->hasDefault()){
        return false;
    }
    return eliminateDeadCode(_switch->getDefault()) && returnsAlways;
}

bool Optimization::DeadCode::eliminateDeadCode(IRCaseStmt* _case){
    return eliminateDeadCode(_case->getSwitchBlock());
}

bool Optimization::DeadCode::eliminateDeadCode(IRDefaultStmt* _default){
    return eliminateDeadCode(_default->getSwitchBlock());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchBlockStmt* _swBlock){
    size_t currentStmtIdx = 0;
    for(auto& stmt : _swBlock->getStatements()){
        if(eliminateDeadCode(stmt.get())){
            _swBlock->eliminateDead(currentStmtIdx + 1);
            return true;
        }
        ++currentStmtIdx;
    }
    return false;
} 

bool Optimization::DeadCode::eliminateDeadCode([[maybe_unused]] IRNode* _node){
    return false;
}