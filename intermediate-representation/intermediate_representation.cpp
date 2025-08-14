#include "intermediate_representation.hpp"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <format>
#include <thread>

#include "../thread-pool/thread_pool.hpp"
#include "defs/ir_defs.hpp"
#include "../common/abstract-syntax-tree/ASTBinaryExpression.hpp"
#include "../common/intermediate-representation-tree/IRParameter.hpp"

IntermediateRepresentation::IntermediateRepresentation() = default;

thread_local IRThreadContext IntermediateRepresentation::irContext;

std::unique_ptr<IRProgram> IntermediateRepresentation::formIR(const ASTProgram* _program){
    std::unique_ptr<IRProgram> _irProgram = std::make_unique<IRProgram>(IRNodeType::PROGRAM);
    
    const size_t total = _program->getFunctionCount();
    _irProgram->resizeFunctions(total);

    std::atomic<size_t> done{};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    std::mutex irMtx;

    for(size_t i = 0; i < total; ++i){
        threadPool.enqueue([&, _irProgram=_irProgram.get(), i, _function=_program->getFunctionAtN(i)]{
            // generating ir of a function
            std::unique_ptr<IRFunction> _irFunction = function(_function);

            {
                // setting function on its position
                std::lock_guard<std::mutex> lock(irMtx);
                _irProgram->setFunctionAtN(std::move(_irFunction), i);
            }

            // check if all ir functions are generated
            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_one();
            }
        });
    }

    {
        // wait untill all ir functions are generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&]{ return done == total; });
    }

    checkErrors(_irProgram.get());

    return _irProgram;
}

void IntermediateRepresentation::checkErrors(const IRProgram* _program) const {
    std::string errors{""};
    for(const auto& _function : _program->getFunctions()){
        const std::vector<std::string>& funcErrors = exceptions.at(_function->getFunctionName());
        for(const auto& err : funcErrors){
            errors += std::format("{}\n", err);
        }
    }
    if(!errors.empty()){
        throw std::runtime_error(errors);
    }
}

std::unique_ptr<IRFunction> IntermediateRepresentation::function(const ASTFunction* _function){
    std::unique_ptr<IRFunction> _irFunction = 
        std::make_unique<IRFunction>(IRNodeType::FUNCTION, _function->getToken().value, _function->getType());
    
    irContext.init();

    parameter(_irFunction.get(), _function->getParameters());
    body(_irFunction.get(), _function->getBody());

    eliminateDeadCode(_irFunction.get());

    // bytes allocated for local variables
    countVariables(_irFunction.get());
    std::string varCountStr{ std::to_string(irContext.requiredMemory(regSize)) };
    _irFunction->setRequiredMemory(varCountStr);

    {
        // pairing function name with its errors
        std::lock_guard<std::mutex> lock(exceptionMtx);
        exceptions[_function->getToken().value] = std::move(irContext.errors); // irContext.errors are cleared after std::move()
    }

    irContext.reset();

    return _irFunction;
}

void IntermediateRepresentation::parameter(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTParameter>>& _parameters){
    for(const auto& _parameter : _parameters){
        _irFunction->addParameter(std::make_unique<IRParameter>(IRNodeType::PARAMETER, _parameter->getToken().value, _parameter->getType()));
    }
}

void IntermediateRepresentation::body(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTStatement>>& _body){
    for(const auto& _statement : _body){
        _irFunction->addStatement(statement(_statement.get()));

        // ignore all constructs after return statement
        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
}

std::unique_ptr<IRStatement> IntermediateRepresentation::statement(const ASTStatement* _statement){
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
            // unreachable, added just to suppress warnings
            irContext.errors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid statement '{}'\n", 
                _statement->getToken().line, _statement->getToken().column, astNodeTypeToString.at(_statement->getNodeType()))
            );
            return nullptr;
    }
}

std::unique_ptr<IRVariable> IntermediateRepresentation::variable(const ASTVariable* _variable){
    std::unique_ptr<IRVariable> _irVariable = std::make_unique<IRVariable>(IRNodeType::VARIABLE, _variable->getToken().value, _variable->getType());
    if(_variable->hasAssign()){
        auto temps{ initiateTemporaries(_variable->getAssign()) };
        _irVariable->setAssign(numericalExpression(_variable->getAssign()), std::move(temps));
    }
    return _irVariable;
}

std::unique_ptr<IRPrintfSt> IntermediateRepresentation::printfStatement(const ASTPrintfSt* _printf){
    std::unique_ptr<IRPrintfSt> _irPrintf = std::make_unique<IRPrintfSt>(IRNodeType::PRINTF);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(_printf->getExp()) };
    _irPrintf->setExp(numericalExpression(_printf->getExp()), std::move(temps));

    return _irPrintf;
}

std::unique_ptr<IRIfSt> IntermediateRepresentation::ifStatement(const ASTIfSt* _if){
    std::unique_ptr<IRIfSt> _irIf = std::make_unique<IRIfSt>(IRNodeType::IF);
    const auto& conditions = _if->getConditions();
    const auto& statements = _if->getStatements();

    const size_t conditionCount = conditions.size();

    for(size_t i = 0; i < conditionCount; ++i){
        auto temps{ initiateTemporaries(conditions[i].get()) };
        _irIf->addIf(relationalExpression(conditions[i].get()), statement(statements[i].get()), std::move(temps));
    }

    if(conditionCount != statements.size()){
        _irIf->addElse(statement(statements.back().get()));
    }

    return _irIf;
}

std::unique_ptr<IRCompoundSt> IntermediateRepresentation::compoundStatement(const ASTCompoundSt* _compound){
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

std::unique_ptr<IRAssignSt> IntermediateRepresentation::assignmentStatement(const ASTAssignSt* _assignment){
    std::unique_ptr<IRAssignSt> _irAssignment = std::make_unique<IRAssignSt>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(_assignment->getExp()) };
    _irAssignment->setAssignSt(id(_assignment->getVariable()), numericalExpression(_assignment->getExp()), std::move(temps));

    return _irAssignment;
}

std::unique_ptr<IRReturnSt> IntermediateRepresentation::returnStatement(const ASTReturnSt* _return){
    std::unique_ptr<IRReturnSt> _irReturn = std::make_unique<IRReturnSt>(IRNodeType::RETURN);
    if(_return->returns()){
        // extracting function calls to temporary variables
        auto temps{ initiateTemporaries(_return->getExp()) };

        _irReturn->setExp(numericalExpression(_return->getExp()), std::move(temps));
    }
    return _irReturn;
}

std::unique_ptr<IRWhileSt> IntermediateRepresentation::whileStatement(const ASTWhileSt* _while){
    std::unique_ptr<IRWhileSt> _irWhile = std::make_unique<IRWhileSt>(IRNodeType::WHILE);
    auto temps{ initiateTemporaries(_while->getCondition()) };
    _irWhile->setWhileSt(relationalExpression(_while->getCondition()), statement(_while->getStatement()), std::move(temps));

    return _irWhile;
}

std::unique_ptr<IRForSt> IntermediateRepresentation::forStatement(const ASTForSt* _for){
    std::unique_ptr<IRForSt> _irFor = std::make_unique<IRForSt>(IRNodeType::FOR);
    
    std::unique_ptr<IRAssignSt> init{ nullptr }, inc{ nullptr };
    std::unique_ptr<IRExpression> condition{ nullptr };
    std::unique_ptr<IRTemporary> temps{ nullptr };

    if(_for->hasInitializer()){
        init = assignmentStatement(_for->getInitializer());
    }
    if(_for->hasCondition()){
        temps = initiateTemporaries(_for->getCondition());
        condition = relationalExpression(_for->getCondition());
    }
    if(_for->hasIncrementer()){
        inc = assignmentStatement(_for->getIncrementer());
    }

    _irFor->setForSt(std::move(init), std::move(condition), std::move(inc), statement(_for->getStatement()), std::move(temps));

    return _irFor;
}

std::unique_ptr<IRDoWhileSt> IntermediateRepresentation::doWhileStatement(const ASTDoWhileSt* _dowhile){
    std::unique_ptr<IRDoWhileSt> _irDowhile = std::make_unique<IRDoWhileSt>(IRNodeType::DO_WHILE);
    auto temps{ initiateTemporaries(_dowhile->getCondition()) };
    _irDowhile->setDoWhileSt(relationalExpression(_dowhile->getCondition()), statement(_dowhile->getStatement()), std::move(temps));

    return _irDowhile;
}

std::unique_ptr<IRSwitchSt> IntermediateRepresentation::switchStatement(const ASTSwitchSt* _switch){
    std::unique_ptr<IRSwitchSt> _irSwitch = std::make_unique<IRSwitchSt>(IRNodeType::SWITCH);

    _irSwitch->setVariable(id(_switch->getVariable()));

    for(const auto& _swCase : _switch->getCases()){
        _irSwitch->addCase(_case(_swCase.get()));
    }
    
    if(_switch->hasDefault()){
        _irSwitch->setDefault(_default(_switch->getDefault()));
    }

    return _irSwitch;
}

std::unique_ptr<IRCaseSt> IntermediateRepresentation::_case(const ASTCaseSt* _astCase){
    std::unique_ptr<IRCaseSt> _irCase = std::make_unique<IRCaseSt>(IRNodeType::CASE);
    _irCase->setCase(literal(_astCase->getLiteral()), switchBlock(_astCase->getSwitchBlock()), _astCase->hasBreak());

    return _irCase;
}

std::unique_ptr<IRDefaultSt> IntermediateRepresentation::_default(const ASTDefaultSt* _astDefault){
    std::unique_ptr<IRDefaultSt> _irDefault = std::make_unique<IRDefaultSt>(IRNodeType::DEFAULT);
    _irDefault->setSwitchBlock(switchBlock(_astDefault->getSwitchBlock()));

    return _irDefault;
}

std::unique_ptr<IRSwitchBlock> IntermediateRepresentation::switchBlock(const ASTSwitchBlock* _block){
    std::unique_ptr<IRSwitchBlock> _irSwBlock = std::make_unique<IRSwitchBlock>(IRNodeType::SWITCH_BLOCK);
    for(const auto& _statement : _block->getStatements()){
        _irSwBlock->addStatement(statement(_statement.get()));

        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    
    return _irSwBlock;
}

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRExpression> IntermediateRepresentation::numericalExpression(const ASTExpression* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        return id(static_cast<const ASTId*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        return literal(static_cast<const ASTLiteral*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCall(static_cast<const ASTFunctionCall*>(_numexp));
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp);
        std::unique_ptr<IRExpression> leftOperand{ numericalExpression(binExp->getLeftOperand()) };
        std::unique_ptr<IRExpression> rightOperand{ numericalExpression(binExp->getRightOperand()) };

        if(leftOperand->getNodeType() == IRNodeType::LITERAL && rightOperand->getNodeType() == IRNodeType::LITERAL){
            if(leftOperand->getType() == Types::INT){
                return mergeLiterals<int>(static_cast<const IRLiteral*>(leftOperand.get()), 
                    static_cast<const IRLiteral*>(rightOperand.get()), binExp
                );
            }
            else if(leftOperand->getType() == Types::UNSIGNED){
                return mergeLiterals<unsigned>(static_cast<const IRLiteral*>(leftOperand.get()), 
                    static_cast<const IRLiteral*>(rightOperand.get()), binExp
                );
            }
        }
        
        std::string val{ _numexp->getToken().value };
        Types type{ leftOperand->getType() };
        IRNodeType iNodeType{ operatorToIRNodeType.at(binExp->getOperator()).getOperation(type) };

        std::unique_ptr<IRBinaryExpression> _irBinExp = std::make_unique<IRBinaryExpression>(iNodeType, type);
        _irBinExp->setBinaryExpression(std::move(leftOperand), std::move(rightOperand), binExp->getOperator());

        return _irBinExp;
    }
}

// reducing the depth of the tree if both operands are literals
template<typename T>
std::unique_ptr<IRExpression> IntermediateRepresentation::mergeLiterals(const IRLiteral* leftOperand, const IRLiteral* rightOperand, const ASTBinaryExpression* binExp) const {
    T lval = (std::is_same<T, int>::value ? std::stoi(leftOperand->getValue()) : std::stoul(leftOperand->getValue()));
    T rval = (std::is_same<T, int>::value ? std::stoi(rightOperand->getValue()) : std::stoul(rightOperand->getValue()));
    T result{ mergeValues<T>(lval, rval, binExp->getOperator(), binExp->getToken().line, binExp->getToken().column) };

    Types type{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };
    std::string suffix{ type == Types::INT ? "" : "u" };

    return std::make_unique<IRLiteral>(IRNodeType::LITERAL, std::to_string(result) + suffix, type);
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, Operators op, size_t line, size_t column) const {
    switch(op){
        case Operators::ADD:
            return l + r;
        case Operators::SUB:
            return l - r;
        case Operators::MUL:
            return l * r;
        case Operators::DIV:
            if(r == 0){
                irContext.errors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> division by ZERO", line, column)
                );
                return 0;
            }
            return l / r;
        case Operators::ANDB:
            return l & r;
        case Operators::ORB:
            return l | r;
        case Operators::XOR:
            return l ^ r;
        case Operators::LSHIFT:
            return l << r;
        case Operators::RSHIFT:
            return l >> r;
        case Operators::GREATER:
            return l > r ? 1 : 0;
        case Operators::LESS:
            return l < r ? 1 : 0;
        case Operators::GEQUAL:
            return l >= r ? 1 : 0;
        case Operators::LEQUAL:
            return l <= r ? 1 : 0;
        case Operators::EQUAL:
            return l == r ? 1 : 0;
        case Operators::NEQUAL:
            return l != r ? 1 : 0;
        case Operators::NO_OP:
            // unreachable
            return 0;
    }
}

std::unique_ptr<IRBinaryExpression> IntermediateRepresentation::relationalExpression(const ASTExpression* _relexp) {
    const ASTBinaryExpression* _astBinExp = static_cast<const ASTBinaryExpression*>(_relexp);
    
    IRNodeType irNodeType{ operatorToIRNodeType.at(_astBinExp->getOperator()).getOperation(_relexp->getType()) };
    std::unique_ptr<IRBinaryExpression> _irBinExp = std::make_unique<IRBinaryExpression>(irNodeType, _astBinExp->getType());

    _irBinExp->setBinaryExpression(numericalExpression(_astBinExp->getLeftOperand()), numericalExpression(_astBinExp->getRightOperand()), _astBinExp->getOperator());
    return _irBinExp;
}

std::unique_ptr<IRId> IntermediateRepresentation::id(const ASTId* _id) const {
    return std::make_unique<IRId>(IRNodeType::ID, _id->getToken().value, _id->getType());
}

std::unique_ptr<IRLiteral> IntermediateRepresentation::literal(const ASTLiteral* _literal) const {
    return std::make_unique<IRLiteral>(IRNodeType::LITERAL, _literal->getToken().value, _literal->getType());
}

std::unique_ptr<IRFunctionCall> IntermediateRepresentation::functionCall(const ASTFunctionCall* _functionCall){
    std::unique_ptr<IRFunctionCall> _irFunctionCall = 
        std::make_unique<IRFunctionCall>(IRNodeType::CALL, _functionCall->getToken().value, _functionCall->getType());
    argument(_irFunctionCall.get(), _functionCall);

    return _irFunctionCall;
}

void IntermediateRepresentation::argument(IRFunctionCall* _irFunctionCall, const ASTFunctionCall* _functionCall){
    for(const auto& argument : _functionCall->getArguments()){
        auto temps{ initiateTemporaries(argument.get()) };
        _irFunctionCall->addArgument(numericalExpression(argument.get()), std::move(temps));
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t IntermediateRepresentation::countTemporaries(const ASTExpression* _numexp) const {
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::ID || _numexp->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp); 
        return countTemporaries(binExp->getLeftOperand()) + countTemporaries(binExp->getRightOperand());
    }
}

// generating temporary variables
std::string IntermediateRepresentation::generateTemporaries(){
    std::string name = std::format("_t{}", ++irContext.temporaries);
    irContext.temporaryNames.push(name);
    return name;
}

// assigning a returned value to temporary variables
void IntermediateRepresentation::assignFunctionCalls(IRTemporary* _temporaryRoot, const ASTExpression* _numexp, size_t& idx){
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        const ASTFunctionCall* funcCall = static_cast<const ASTFunctionCall*>(_numexp);

        _temporaryRoot->assignTempAtN(functionCall(funcCall), _numexp->getType(), idx++);
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::VARIABLE || _numexp->getNodeType() == ASTNodeType::ID){
        return;
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp);
        assignFunctionCalls(_temporaryRoot, binExp->getLeftOperand(), idx);
        assignFunctionCalls(_temporaryRoot, binExp->getRightOperand(), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRId> IntermediateRepresentation::replaceFunctionCall(const ASTFunctionCall* _functionCall){
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRId>(IRNodeType::ID, name, _functionCall->getType());
}

std::unique_ptr<IRTemporary> IntermediateRepresentation::initiateTemporaries(const ASTExpression* _numexp){
    size_t tmpCount = countTemporaries(_numexp);
    if(tmpCount > 0){
        std::unique_ptr<IRTemporary> temporaryRoot = std::make_unique<IRTemporary>(IRNodeType::TEMPORARY);
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->addTemporary(generateTemporaries());
        }
        assignFunctionCalls(temporaryRoot.get(), _numexp, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}

bool IntermediateRepresentation::eliminateDeadCode(IRNode* _construct){
    switch(_construct->getNodeType()){
        case IRNodeType::RETURN:
            return true;
        case IRNodeType::FUNCTION:{
            auto _function{static_cast<IRFunction*>(_construct)};
            size_t currentStmtIdx = 0;
            for(auto& stmt : _function->getBody()){
                if(eliminateDeadCode(stmt.get())){
                    _function->eliminateDead(currentStmtIdx + 1);
                    return true;
                }
                ++currentStmtIdx;
            }
            return false;
        }
        case IRNodeType::COMPOUND: {
            auto _compound{ static_cast<IRCompoundSt*>(_construct) };
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
        case IRNodeType::DEFAULT:{
            auto _default{ static_cast<IRDefaultSt*>(_construct) };
            return eliminateDeadCode(_default->getSwitchBlockNC());
        }
        case IRNodeType::CASE: {
            auto _case{ static_cast<IRCaseSt*>(_construct) };
            return eliminateDeadCode(_case->getSwitchBlockNC());
        }
        case IRNodeType::SWITCH_BLOCK: {
            auto switchBlock{ static_cast<IRSwitchBlock*>(_construct) };
            size_t currentStmtIdx = 0;
            for(auto& stmt : switchBlock->getStatements()){
                if(eliminateDeadCode(stmt.get())){
                    switchBlock->eliminateDead(currentStmtIdx + 1);
                    return true;
                }
                ++currentStmtIdx;
            }
            return false;   
        }
        case IRNodeType::IF: {
            auto _if{ static_cast<IRIfSt*>(_construct) };
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
        case IRNodeType::SWITCH: {
            auto _switch{ static_cast<IRSwitchSt*>(_construct) };
            bool returnsAlways = true;
            for(auto& _case : _switch->getCases()){
                if(!eliminateDeadCode(_case.get()) && _case->hasBreak()){
                    returnsAlways = false;
                }
            }
            if(!_switch->hasDefault()){
                return false;
            }
            return eliminateDeadCode(_switch->getDefaultNC()) && returnsAlways;
        }
        case IRNodeType::DO_WHILE: {
            auto _dowhile{ static_cast<IRDoWhileSt*>(_construct) };
            return eliminateDeadCode(_dowhile->getStatementNC());
        }
        default:
            return false;
    }
}

void IntermediateRepresentation::countVariables(const IRNode* _construct){
    auto nodeType{ _construct->getNodeType() };
    switch(nodeType){
        case IRNodeType::VARIABLE:{
            ++irContext.variableCount;
            const auto& _var = static_cast<const IRVariable*>(_construct);
            if(_var->hasTemporaries()){
                countVariables(_var->getTemporaries());
            }
            return;
        }
        case IRNodeType::FUNCTION:
            for(const auto& _statement : static_cast<const IRFunction*>(_construct)->getBody()){
                countVariables(_statement.get());
            }
            return;
        case IRNodeType::COMPOUND:
            for(const auto& _statement : static_cast<const IRCompoundSt*>(_construct)->getStatements()){
                countVariables(_statement.get());
            }
            return;
        case IRNodeType::SWITCH:{
            const auto _switch = static_cast<const IRSwitchSt*>(_construct);
            for(const auto& _case : _switch->getCases()){
                countVariables(_case->getSwitchBlock());
            }
            if(_switch->hasDefault()){
                countVariables(_switch->getDefault()->getSwitchBlock());
            }
            return;
        }
        case IRNodeType::SWITCH_BLOCK:
            for(const auto& _statement : static_cast<const IRSwitchBlock*>(_construct)->getStatements()){
                countVariables(_statement.get());
            }
            return;
        case IRNodeType::IF:{
            const auto _if = static_cast<const IRIfSt*>(_construct);
            for(const auto& _statement : _if->getStatements()){
                countVariables(_statement.get());
            }
            for(const auto& temp : _if->getTemporaries()){
                if(temp != nullptr){
                    countVariables(temp.get());
                }
            }
            return;
        }
        case IRNodeType::FOR: {
            const auto _for = static_cast<const IRForSt*>(_construct);
            countVariables(_for->getStatement());
            if(_for->hasTemporaries()){
                countVariables(_for->getTemporaries());
            }
            return;
        }
        case IRNodeType::WHILE:{
            const auto _while = static_cast<const IRWhileSt*>(_construct);
            countVariables(_while->getStatement());
            if(_while->hasTemporaries()){
                countVariables(_while->getTemporaries());
            }
            return;
        }
        case IRNodeType::DO_WHILE:{
            const auto _dowhile = static_cast<const IRDoWhileSt*>(_construct);
            countVariables(_dowhile->getStatement());
            if(_dowhile->hasTemporaries()){
                countVariables(_dowhile->getTemporaries());
            }
            return;
        }
        case IRNodeType::PRINTF: {
            const auto _printf = static_cast<const IRPrintfSt*>(_construct);
            if(_printf->hasTemporaries()){
                countVariables(_printf->getTemporaries());
            }
            return;
        }
        case IRNodeType::ASSIGN: {
            const auto _assign = static_cast<const IRAssignSt*>(_construct);
            if(_assign->hasTemporaries()){
                countVariables(_assign->getTemporaries());
            }
            return;
        }
        case IRNodeType::RETURN: {
            const auto _ret = static_cast<const IRReturnSt*>(_construct);
            if(_ret->hasTemporaries()){
                countVariables(_ret->getTemporaries());
            }
            return;
        }
        case IRNodeType::CALL:
            for(const auto& temp : static_cast<const IRFunctionCall*>(_construct)->getTemporaries()){
                if(temp != nullptr){
                    countVariables(temp.get());
                }
            }
            return;
        case IRNodeType::TEMPORARY:
            for(const auto& _exp : static_cast<const IRTemporary*>(_construct)->getTemporaries()){
                ++irContext.variableCount;
                if(_exp->getNodeType() == IRNodeType::CALL){
                    countVariables(_exp.get());
                }
            }
            return;
        default:
            return;
    }
}
