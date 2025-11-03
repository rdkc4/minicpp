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

bool Optimization::DeadCode::eliminateDeadCode(IRStatement* _stmt){
    switch(_stmt->getNodeType()){
        case IRNodeType::COMPOUND:
            return eliminateDeadCode(static_cast<IRCompoundSt*>(_stmt));
        case IRNodeType::RETURN:
            return eliminateDeadCode(static_cast<IRReturnSt*>(_stmt));
        case IRNodeType::IF:
            return eliminateDeadCode(static_cast<IRIfSt*>(_stmt));
        case IRNodeType::DO_WHILE:
            return eliminateDeadCode(static_cast<IRDoWhileSt*>(_stmt));
        case IRNodeType::SWITCH:
            return eliminateDeadCode(static_cast<IRSwitchSt*>(_stmt));
        default:
            return false;
    }
}

bool Optimization::DeadCode::eliminateDeadCode(IRCompoundSt* _compound){
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

bool Optimization::DeadCode::eliminateDeadCode([[maybe_unused]] IRReturnSt* _return){
    return true;
}

bool Optimization::DeadCode::eliminateDeadCode(IRIfSt* _if){
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

bool Optimization::DeadCode::eliminateDeadCode(IRDoWhileSt* _dowhile){
    return eliminateDeadCode(_dowhile->getStatement());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchSt* _switch){
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

bool Optimization::DeadCode::eliminateDeadCode(IRCaseSt* _case){
    return eliminateDeadCode(_case->getSwitchBlock());
}

bool Optimization::DeadCode::eliminateDeadCode(IRDefaultSt* _default){
    return eliminateDeadCode(_default->getSwitchBlock());
}

bool Optimization::DeadCode::eliminateDeadCode(IRSwitchBlock* _swBlock){
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