#include "../stack_memory.hpp"

size_t Optimization::StackMemory::computeStackMemory(const IRFunction* _function){
    size_t variableCount = 0;
    for(const auto& _statement : _function->getBody()){
        variableCount += computeStackMemory(_statement.get());
    }
    return variableCount * regSize;
}

size_t Optimization::StackMemory::computeStackMemory(const IRTemporary* _temporary){
    size_t variableCount = 0;
    for(const auto& _exp : _temporary->getTemporaries()){
        ++variableCount;
        if(_exp->getNodeType() == IRNodeType::CALL){
            variableCount += computeStackMemory(_exp.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRStatement* _stmt){
    switch(_stmt->getNodeType()){
        case IRNodeType::VARIABLE:
            return computeStackMemory(static_cast<const IRVariable*>(_stmt));
        case  IRNodeType::COMPOUND:
            return computeStackMemory(static_cast<const IRCompoundSt*>(_stmt));
        case  IRNodeType::IF:
            return computeStackMemory(static_cast<const IRIfSt*>(_stmt));
        case  IRNodeType::FOR:
            return computeStackMemory(static_cast<const IRForSt*>(_stmt));
        case  IRNodeType::WHILE:
            return computeStackMemory(static_cast<const IRWhileSt*>(_stmt));
        case  IRNodeType::DO_WHILE:
            return computeStackMemory(static_cast<const IRDoWhileSt*>(_stmt));
        case  IRNodeType::PRINTF:
            return computeStackMemory(static_cast<const IRPrintfSt*>(_stmt));
        case  IRNodeType::ASSIGN:
            return computeStackMemory(static_cast<const IRAssignSt*>(_stmt));
        case  IRNodeType::RETURN:
            return computeStackMemory(static_cast<const IRReturnSt*>(_stmt));
        case  IRNodeType::SWITCH:
            return computeStackMemory(static_cast<const IRSwitchSt*>(_stmt));
        default:
            return 0;
    }
}

size_t Optimization::StackMemory::computeStackMemory(const IRVariable* _variable){
    size_t variableCount = 1;
    if(_variable->hasTemporaries()){
        variableCount += computeStackMemory(_variable->getTemporaries());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRCompoundSt* _compound){
    size_t variableCount = 0;
    for(const auto& _statement : _compound->getStatements()){
        variableCount += computeStackMemory(_statement.get());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRIfSt* _if){
    size_t variableCount = 0;
    for(const auto& _statement : _if->getStatements()){
        variableCount += computeStackMemory(_statement.get());
    }
    for(const auto& temp : _if->getTemporaries()){
        if(temp != nullptr){
            variableCount += computeStackMemory(temp.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRForSt* _for){
    size_t variableCount = computeStackMemory(_for->getStatement());
    if(_for->hasTemporaries()){
        variableCount += computeStackMemory(_for->getTemporaries());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRWhileSt* _while){
    size_t variableCount = computeStackMemory(_while->getStatement());
    if(_while->hasTemporaries()){
        variableCount += computeStackMemory(_while->getTemporaries());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRDoWhileSt* _dowhile){
    size_t variableCount = computeStackMemory(_dowhile->getStatement());
    if(_dowhile->hasTemporaries()){
        variableCount += computeStackMemory(_dowhile->getTemporaries());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRPrintfSt* _printf){
    if(_printf->hasTemporaries()){
        return computeStackMemory(_printf->getTemporaries());
    }
    return 0;
}

size_t Optimization::StackMemory::computeStackMemory(const IRAssignSt* _assign){
    if(_assign->hasTemporaries()){
        return computeStackMemory(_assign->getTemporaries());
    }
    return 0;
}

size_t Optimization::StackMemory::computeStackMemory(const IRReturnSt* _return){
    if(_return->hasTemporaries()){
        return computeStackMemory(_return->getTemporaries());
    }
    return 0;
}

size_t Optimization::StackMemory::computeStackMemory(const IRSwitchSt* _switch){
    size_t variableCount = 0;
    for(const auto& _case : _switch->getCases()){
        variableCount += computeStackMemory(_case->getSwitchBlock());
    }
    if(_switch->hasDefault()){
        variableCount += computeStackMemory(_switch->getDefault()->getSwitchBlock());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRSwitchBlock* _swBlock){
    size_t variableCount = 0;
    for(const auto& _statement : _swBlock->getStatements()){
        variableCount += computeStackMemory(_statement.get());
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory(const IRFunctionCall* _functionCall){
    size_t variableCount = 0;
    for(const auto& temp : _functionCall->getTemporaries()){
        if(temp != nullptr){
            variableCount += computeStackMemory(temp.get());
        }
    }
    return variableCount;
}

size_t Optimization::StackMemory::computeStackMemory([[maybe_unused]] const IRNode* _node){
    return 0;
}