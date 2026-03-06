#include "../statement_intermediate_representation.hpp"

#include <cassert>
#include <utility>

StatementIntermediateRepresentation::StatementIntermediateRepresentation() = default;

std::unique_ptr<IRStmt> StatementIntermediateRepresentation::statement(const ASTStmt* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::VARIABLE:
            return variable(static_cast<const ASTVariableDeclStmt*>(_statement));
        case ASTNodeType::IF_STATEMENT:
            return ifStatement(static_cast<const ASTIfStmt*>(_statement));
        case ASTNodeType::COMPOUND_STATEMENT:
            return compoundStatement(static_cast<const ASTCompoundStmt*>(_statement));
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            return assignmentStatement(static_cast<const ASTAssignStmt*>(_statement));
        case ASTNodeType::RETURN_STATEMENT:
            return returnStatement(static_cast<const ASTReturnStmt*>(_statement));
        case ASTNodeType::WHILE_STATEMENT:
            return whileStatement(static_cast<const ASTWhileStmt*>(_statement));
        case ASTNodeType::FOR_STATEMENT:
            return forStatement(static_cast<const ASTForStmt*>(_statement));
        case ASTNodeType::DO_WHILE_STATEMENT:
            return doWhileStatement(static_cast<const ASTDoWhileStmt*>(_statement));
        case ASTNodeType::SWITCH_STATEMENT:
            return switchStatement(static_cast<const ASTSwitchStmt*>(_statement));
        case ASTNodeType::FUNCTION_CALL_STATEMENT:
            return functionCallStatement(static_cast<const ASTFunctionCallStmt*>(_statement));
        default:
            std::unreachable();
    }
}

std::unique_ptr<IRVariableDeclStmt> StatementIntermediateRepresentation::variable(const ASTVariableDeclStmt* _variable){
    std::unique_ptr<IRVariableDeclStmt> _irVariable = std::make_unique<IRVariableDeclStmt>(IRNodeType::VARIABLE, _variable->getToken().value, _variable->getType());
    if(_variable->hasAssign()){
        auto temps{ expIR.initiateTemporaries(_variable->getAssign()) };
        _irVariable->setAssign(expIR.numericalExpression(_variable->getAssign()), std::move(temps));
    }
    return _irVariable;
}

std::unique_ptr<IRIfStmt> StatementIntermediateRepresentation::ifStatement(const ASTIfStmt* _if){
    std::unique_ptr<IRIfStmt> _irIf = std::make_unique<IRIfStmt>(IRNodeType::IF);
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

std::unique_ptr<IRCompoundStmt> StatementIntermediateRepresentation::compoundStatement(const ASTCompoundStmt* _compound){
    std::unique_ptr<IRCompoundStmt> _irCompound = std::make_unique<IRCompoundStmt>(IRNodeType::COMPOUND);
    for(const auto& _statement : _compound->getStatements()){
        _irCompound->addStatement(statement(_statement.get()));

        // ignore all constructs after return statement
        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    return _irCompound;
}

std::unique_ptr<IRAssignStmt> StatementIntermediateRepresentation::assignmentStatement(const ASTAssignStmt* _assignment){
    std::unique_ptr<IRAssignStmt> _irAssignment = std::make_unique<IRAssignStmt>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ expIR.initiateTemporaries(_assignment->getExp()) };
    _irAssignment->setAssignSt(expIR.id(_assignment->getVariable()), expIR.numericalExpression(_assignment->getExp()), std::move(temps));

    return _irAssignment;
}

std::unique_ptr<IRReturnStmt> StatementIntermediateRepresentation::returnStatement(const ASTReturnStmt* _return){
    std::unique_ptr<IRReturnStmt> _irReturn = std::make_unique<IRReturnStmt>(IRNodeType::RETURN);
    if(_return->returns()){
        // extracting function calls to temporary variables
        auto temps{ expIR.initiateTemporaries(_return->getExp()) };

        _irReturn->setExp(expIR.numericalExpression(_return->getExp()), std::move(temps));
    }
    return _irReturn;
}

std::unique_ptr<IRFunctionCallStmt> StatementIntermediateRepresentation::functionCallStatement(const ASTFunctionCallStmt* _call){
    std::unique_ptr<IRFunctionCallStmt> _irCall = std::make_unique<IRFunctionCallStmt>(IRNodeType::CALL);
    _irCall->setFunctionCallSt(expIR.functionCall(_call->getFunctionCall()));

    return _irCall;
}

std::unique_ptr<IRWhileStmt> StatementIntermediateRepresentation::whileStatement(const ASTWhileStmt* _while){
    std::unique_ptr<IRWhileStmt> _irWhile = std::make_unique<IRWhileStmt>(IRNodeType::WHILE);
    auto temps{ expIR.initiateTemporaries(_while->getCondition()) };
    _irWhile->setWhileSt(expIR.relationalExpression(_while->getCondition()), statement(_while->getStatement()), std::move(temps));

    return _irWhile;
}

std::unique_ptr<IRForStmt> StatementIntermediateRepresentation::forStatement(const ASTForStmt* _for){
    std::unique_ptr<IRForStmt> _irFor = std::make_unique<IRForStmt>(IRNodeType::FOR);
    
    std::unique_ptr<IRAssignStmt> init{ nullptr }, inc{ nullptr };
    std::unique_ptr<IRExpr> condition{ nullptr };
    std::unique_ptr<IRTemporaryExpr> temps{ nullptr };

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

std::unique_ptr<IRDoWhileStmt> StatementIntermediateRepresentation::doWhileStatement(const ASTDoWhileStmt* _dowhile){
    std::unique_ptr<IRDoWhileStmt> _irDowhile = std::make_unique<IRDoWhileStmt>(IRNodeType::DO_WHILE);
    auto temps{ expIR.initiateTemporaries(_dowhile->getCondition()) };
    _irDowhile->setDoWhileSt(expIR.relationalExpression(_dowhile->getCondition()), statement(_dowhile->getStatement()), std::move(temps));

    return _irDowhile;
}

std::unique_ptr<IRSwitchStmt> StatementIntermediateRepresentation::switchStatement(const ASTSwitchStmt* _switch){
    std::unique_ptr<IRSwitchStmt> _irSwitch = std::make_unique<IRSwitchStmt>(IRNodeType::SWITCH);

    _irSwitch->setVariable(expIR.id(_switch->getVariable()));

    for(const auto& _swCase : _switch->getCases()){
        _irSwitch->addCase(_case(_swCase.get()));
    }
    
    if(_switch->hasDefault()){
        _irSwitch->setDefault(_default(_switch->getDefault()));
    }

    return _irSwitch;
}

std::unique_ptr<IRCaseStmt> StatementIntermediateRepresentation::_case(const ASTCaseStmt* _astCase){
    std::unique_ptr<IRCaseStmt> _irCase = std::make_unique<IRCaseStmt>(IRNodeType::CASE);
    _irCase->setCase(expIR.literal(_astCase->getLiteral()), switchBlock(_astCase->getSwitchBlock()), _astCase->hasBreak());

    return _irCase;
}

std::unique_ptr<IRDefaultStmt> StatementIntermediateRepresentation::_default(const ASTDefaultStmt* _astDefault){
    std::unique_ptr<IRDefaultStmt> _irDefault = std::make_unique<IRDefaultStmt>(IRNodeType::DEFAULT);
    _irDefault->setSwitchBlock(switchBlock(_astDefault->getSwitchBlock()));

    return _irDefault;
}

std::unique_ptr<IRSwitchBlockStmt> StatementIntermediateRepresentation::switchBlock(const ASTSwitchBlockStmt* _block){
    std::unique_ptr<IRSwitchBlockStmt> _irSwBlock = std::make_unique<IRSwitchBlockStmt>(IRNodeType::SWITCH_BLOCK);
    for(const auto& _statement : _block->getStatements()){
        _irSwBlock->addStatement(statement(_statement.get()));

        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    
    return _irSwBlock;
}