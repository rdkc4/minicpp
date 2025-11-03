#include "../statement_intermediate_representation.hpp"

#include <cassert>

StatementIntermediateRepresentation::StatementIntermediateRepresentation() = default;

std::unique_ptr<IRStatement> StatementIntermediateRepresentation::statement(const ASTStatement* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::VARIABLE:
            return variable(static_cast<const ASTVariable*>(_statement));
        case ASTNodeType::PRINTF:
            return printfStatement(static_cast<const ASTPrintfSt*>(_statement));
        case ASTNodeType::IF_STATEMENT:
            return ifStatement(static_cast<const ASTIfSt*>(_statement));
        case ASTNodeType::COMPOUND_STATEMENT:
            return compoundStatement(static_cast<const ASTCompoundSt*>(_statement));
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            return assignmentStatement(static_cast<const ASTAssignSt*>(_statement));
        case ASTNodeType::RETURN_STATEMENT:
            return returnStatement(static_cast<const ASTReturnSt*>(_statement));
        case ASTNodeType::WHILE_STATEMENT:
            return whileStatement(static_cast<const ASTWhileSt*>(_statement));
        case ASTNodeType::FOR_STATEMENT:
            return forStatement(static_cast<const ASTForSt*>(_statement));
        case ASTNodeType::DO_WHILE_STATEMENT:
            return doWhileStatement(static_cast<const ASTDoWhileSt*>(_statement));
        case ASTNodeType::SWITCH_STATEMENT:
            return switchStatement(static_cast<const ASTSwitchSt*>(_statement));
        default:
            assert(false && "unreachable");
    }
}

std::unique_ptr<IRVariable> StatementIntermediateRepresentation::variable(const ASTVariable* _variable){
    std::unique_ptr<IRVariable> _irVariable = std::make_unique<IRVariable>(IRNodeType::VARIABLE, _variable->getToken().value, _variable->getType());
    if(_variable->hasAssign()){
        auto temps{ expIR.initiateTemporaries(_variable->getAssign()) };
        _irVariable->setAssign(expIR.numericalExpression(_variable->getAssign()), std::move(temps));
    }
    return _irVariable;
}

std::unique_ptr<IRPrintfSt> StatementIntermediateRepresentation::printfStatement(const ASTPrintfSt* _printf){
    std::unique_ptr<IRPrintfSt> _irPrintf = std::make_unique<IRPrintfSt>(IRNodeType::PRINTF);

    // extracting function calls to temporary variables
    auto temps{ expIR.initiateTemporaries(_printf->getExp()) };
    _irPrintf->setExp(expIR.numericalExpression(_printf->getExp()), std::move(temps));

    return _irPrintf;
}

std::unique_ptr<IRIfSt> StatementIntermediateRepresentation::ifStatement(const ASTIfSt* _if){
    std::unique_ptr<IRIfSt> _irIf = std::make_unique<IRIfSt>(IRNodeType::IF);
    const auto& conditions = _if->getConditions();
    const auto& statements = _if->getStatements();

    const size_t conditionCount = conditions.size();

    for(size_t i = 0; i < conditionCount; ++i){
        auto temps{ expIR.initiateTemporaries(conditions[i].get()) };
        _irIf->addIf(expIR.relationalExpression(conditions[i].get()), statement(statements[i].get()), std::move(temps));
    }

    if(conditionCount != statements.size()){
        _irIf->addElse(statement(statements.back().get()));
    }

    return _irIf;
}

std::unique_ptr<IRCompoundSt> StatementIntermediateRepresentation::compoundStatement(const ASTCompoundSt* _compound){
    std::unique_ptr<IRCompoundSt> _irCompound = std::make_unique<IRCompoundSt>(IRNodeType::COMPOUND);
    for(const auto& _statement : _compound->getStatements()){
        _irCompound->addStatement(statement(_statement.get()));

        // ignore all constructs after return statement
        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    return _irCompound;
}

std::unique_ptr<IRAssignSt> StatementIntermediateRepresentation::assignmentStatement(const ASTAssignSt* _assignment){
    std::unique_ptr<IRAssignSt> _irAssignment = std::make_unique<IRAssignSt>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ expIR.initiateTemporaries(_assignment->getExp()) };
    _irAssignment->setAssignSt(expIR.id(_assignment->getVariable()), expIR.numericalExpression(_assignment->getExp()), std::move(temps));

    return _irAssignment;
}

std::unique_ptr<IRReturnSt> StatementIntermediateRepresentation::returnStatement(const ASTReturnSt* _return){
    std::unique_ptr<IRReturnSt> _irReturn = std::make_unique<IRReturnSt>(IRNodeType::RETURN);
    if(_return->returns()){
        // extracting function calls to temporary variables
        auto temps{ expIR.initiateTemporaries(_return->getExp()) };

        _irReturn->setExp(expIR.numericalExpression(_return->getExp()), std::move(temps));
    }
    return _irReturn;
}

std::unique_ptr<IRWhileSt> StatementIntermediateRepresentation::whileStatement(const ASTWhileSt* _while){
    std::unique_ptr<IRWhileSt> _irWhile = std::make_unique<IRWhileSt>(IRNodeType::WHILE);
    auto temps{ expIR.initiateTemporaries(_while->getCondition()) };
    _irWhile->setWhileSt(expIR.relationalExpression(_while->getCondition()), statement(_while->getStatement()), std::move(temps));

    return _irWhile;
}

std::unique_ptr<IRForSt> StatementIntermediateRepresentation::forStatement(const ASTForSt* _for){
    std::unique_ptr<IRForSt> _irFor = std::make_unique<IRForSt>(IRNodeType::FOR);
    
    std::unique_ptr<IRAssignSt> init{ nullptr }, inc{ nullptr };
    std::unique_ptr<IRExpression> condition{ nullptr };
    std::unique_ptr<IRTemporary> temps{ nullptr };

    if(_for->hasInitializer()){
        init = assignmentStatement(_for->getInitializer());
    }
    if(_for->hasCondition()){
        temps = expIR.initiateTemporaries(_for->getCondition());
        condition = expIR.relationalExpression(_for->getCondition());
    }
    if(_for->hasIncrementer()){
        inc = assignmentStatement(_for->getIncrementer());
    }

    _irFor->setForSt(std::move(init), std::move(condition), std::move(inc), statement(_for->getStatement()), std::move(temps));

    return _irFor;
}

std::unique_ptr<IRDoWhileSt> StatementIntermediateRepresentation::doWhileStatement(const ASTDoWhileSt* _dowhile){
    std::unique_ptr<IRDoWhileSt> _irDowhile = std::make_unique<IRDoWhileSt>(IRNodeType::DO_WHILE);
    auto temps{ expIR.initiateTemporaries(_dowhile->getCondition()) };
    _irDowhile->setDoWhileSt(expIR.relationalExpression(_dowhile->getCondition()), statement(_dowhile->getStatement()), std::move(temps));

    return _irDowhile;
}

std::unique_ptr<IRSwitchSt> StatementIntermediateRepresentation::switchStatement(const ASTSwitchSt* _switch){
    std::unique_ptr<IRSwitchSt> _irSwitch = std::make_unique<IRSwitchSt>(IRNodeType::SWITCH);

    _irSwitch->setVariable(expIR.id(_switch->getVariable()));

    for(const auto& _swCase : _switch->getCases()){
        _irSwitch->addCase(_case(_swCase.get()));
    }
    
    if(_switch->hasDefault()){
        _irSwitch->setDefault(_default(_switch->getDefault()));
    }

    return _irSwitch;
}

std::unique_ptr<IRCaseSt> StatementIntermediateRepresentation::_case(const ASTCaseSt* _astCase){
    std::unique_ptr<IRCaseSt> _irCase = std::make_unique<IRCaseSt>(IRNodeType::CASE);
    _irCase->setCase(expIR.literal(_astCase->getLiteral()), switchBlock(_astCase->getSwitchBlock()), _astCase->hasBreak());

    return _irCase;
}

std::unique_ptr<IRDefaultSt> StatementIntermediateRepresentation::_default(const ASTDefaultSt* _astDefault){
    std::unique_ptr<IRDefaultSt> _irDefault = std::make_unique<IRDefaultSt>(IRNodeType::DEFAULT);
    _irDefault->setSwitchBlock(switchBlock(_astDefault->getSwitchBlock()));

    return _irDefault;
}

std::unique_ptr<IRSwitchBlock> StatementIntermediateRepresentation::switchBlock(const ASTSwitchBlock* _block){
    std::unique_ptr<IRSwitchBlock> _irSwBlock = std::make_unique<IRSwitchBlock>(IRNodeType::SWITCH_BLOCK);
    for(const auto& _statement : _block->getStatements()){
        _irSwBlock->addStatement(statement(_statement.get()));

        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    
    return _irSwBlock;
}