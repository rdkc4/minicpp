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

IntermediateRepresentation::IntermediateRepresentation() = default;

thread_local IRThreadContext IntermediateRepresentation::irContext;

std::unique_ptr<IRTree> IntermediateRepresentation::formIR(const ASTree* _root){
    std::unique_ptr<IRTree> root = std::make_unique<IRTree>(IRNodeType::PROGRAM);
    const ASTree* _functionList = _root->getChild(0);
    
    size_t total = _functionList->getChildren().size();
    root->resizeChildren(total);

    std::atomic<size_t> done{};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    std::mutex irMtx;

    for(size_t i = 0; i < total; ++i){
        threadPool.enqueue([&, root=root.get(), i, _function=_functionList->getChild(i)]{
            // generating ir of a function
            std::unique_ptr<IRTree> _irFunction = function(_function);

            {
                // setting function on its position
                std::lock_guard<std::mutex> lock(irMtx);
                root->setChild(std::move(_irFunction), i);
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

    checkErrors(_functionList);

    return root;
}

void IntermediateRepresentation::checkErrors(const ASTree* _functionList) const {
    std::string errors{""};
    for(const auto& _function : _functionList->getChildren()){
        const std::vector<std::string>& funcErrors = exceptions.at(_function->getToken().value);
        for(const auto& err : funcErrors){
            errors += std::format("{}\n", err);
        }
    }
    if(!errors.empty()){
        throw std::runtime_error(errors);
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::function(const ASTree* _function){
    std::unique_ptr<IRTree> _irFunction = 
        std::make_unique<IRTree>(_function->getToken().value, "", _function->getType(), IRNodeType::FUNCTION);
    
    irContext.init();

    _irFunction->pushChild(parameter(_function->getChild(0)));

    for(const auto& _construct : _function->getChild(1)->getChildren()){
        _irFunction->pushChild(construct(_construct.get()));
        // ignore all constructs after return statement
        if(_irFunction->getChildren().back()->getNodeType() == IRNodeType::RETURN){
            break;
        }
    }

    // bytes allocated for local variables
    std::string varCountStr{ std::to_string(irContext.requiredMemory(regSize)) };
    _irFunction->setValue(varCountStr);

    {
        // pairing function name with its errors
        std::lock_guard<std::mutex> lock(exceptionMtx);
        exceptions[_function->getToken().value] = std::move(irContext.errors); // irContext.errors are cleared after std::move()
    }

    irContext.reset();

    return _irFunction;
}

std::unique_ptr<IRTree> IntermediateRepresentation::parameter(const ASTree* _parameters) const {
    std::unique_ptr<IRTree> _irParameters = std::make_unique<IRTree>(IRNodeType::PARAMETER);
    for(const auto& _parameter : _parameters->getChildren()){
        _irParameters->pushChild(std::make_unique<IRTree>(std::string{_parameter->getToken().value}, "0", _parameter->getType(), IRNodeType::ID));
    }
    return _irParameters;
}

std::unique_ptr<IRTree> IntermediateRepresentation::construct(const ASTree* _construct){
    if(_construct->getNodeType() == ASTNodeType::VARIABLE){
        return variable(_construct);
    }
    else{
        return statement(_construct);
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::variable(const ASTree* _variable){
    std::unique_ptr<IRTree> _irVariable = std::make_unique<IRTree>(std::string{_variable->getToken().value}, "0", _variable->getType(), IRNodeType::VARIABLE);
    if(_variable->getChildren().size() != 0){
        _irVariable->pushChild(assignmentStatement(_variable->getChild(0)));
    }
    ++irContext.variableCount;

    return _irVariable;
}

std::unique_ptr<IRTree> IntermediateRepresentation::statement(const ASTree* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::PRINTF:
            return printfStatement(_statement);
        case ASTNodeType::IF_STATEMENT:
            return ifStatement(_statement);
        case ASTNodeType::COMPOUND_STATEMENT:
            return compoundStatement(_statement);
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            return assignmentStatement(_statement);
        case ASTNodeType::RETURN_STATEMENT:
            return returnStatement(_statement);
        case ASTNodeType::WHILE_STATEMENT:
            return whileStatement(_statement);
        case ASTNodeType::FOR_STATEMENT:
            return forStatement(_statement);
        case ASTNodeType::DO_WHILE_STATEMENT:
            return doWhileStatement(_statement);
        case ASTNodeType::SWITCH_STATEMENT:
            return switchStatement(_statement);
        default:
            // unreachable, added just to suppress warnings
            irContext.errors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid statement '{}'\n", 
                _statement->getToken().line, _statement->getToken().column, astNodeTypeToString.at(_statement->getNodeType()))
            );
            return nullptr;
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::printfStatement(const ASTree* _printf){
    std::unique_ptr<IRTree> _irPrintf = std::make_unique<IRTree>(IRNodeType::PRINTF);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(_printf->getChild(0)) };
    if(temps){
        _irPrintf->pushChild(std::move(temps));
    }
    _irPrintf->pushChild(numericalExpression(_printf->getChild(0)));

    return _irPrintf;
}

std::unique_ptr<IRTree> IntermediateRepresentation::ifStatement(const ASTree* _if){
    std::unique_ptr<IRTree> _irIf = std::make_unique<IRTree>(IRNodeType::IF);
    for(const auto& child : _if->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            _irIf->pushChild(relationalExpression(child.get()));
        }
        else{
            _irIf->pushChild(construct(child.get()));
        }
    }
    return _irIf;
}

std::unique_ptr<IRTree> IntermediateRepresentation::compoundStatement(const ASTree* _compound){
    std::unique_ptr<IRTree> _irCompound = std::make_unique<IRTree>(IRNodeType::COMPOUND);
    for(const auto& _construct : _compound->getChildren()){
        _irCompound->pushChild(construct(_construct.get()));
        // ignore all constructs after return statement
        if(_irCompound->getChildren().back()->getNodeType() == IRNodeType::RETURN){
            break;
        }
    }
    return _irCompound;
}

std::unique_ptr<IRTree> IntermediateRepresentation::assignmentStatement(const ASTree* _assignment){
    std::unique_ptr<IRTree> _irAssignment = std::make_unique<IRTree>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(_assignment->getChild(1)) };
    if(temps){
        _irAssignment->pushChild(std::move(temps));
    }
    
    _irAssignment->pushChild(id(_assignment->getChild(0)));
    _irAssignment->pushChild(numericalExpression(_assignment->getChild(1)));
    return _irAssignment;
}

std::unique_ptr<IRTree> IntermediateRepresentation::returnStatement(const ASTree* _return){
    std::unique_ptr<IRTree> _irReturn = std::make_unique<IRTree>(IRNodeType::RETURN);
    if(_return->getChildren().size() != 0){
        // extracting function calls to temporary variables
        auto temps{ initiateTemporaries(_return->getChild(0)) };
        if(temps){
            _irReturn->pushChild(std::move(temps));
        }

        _irReturn->pushChild(numericalExpression(_return->getChild(0)));
    }
    return _irReturn;
}

std::unique_ptr<IRTree> IntermediateRepresentation::whileStatement(const ASTree* _while){
    std::unique_ptr<IRTree> _irWhile = std::make_unique<IRTree>(IRNodeType::WHILE);
    _irWhile->pushChild(relationalExpression(_while->getChild(0)));
    _irWhile->pushChild(construct(_while->getChild(1)));
    return _irWhile;
}

std::unique_ptr<IRTree> IntermediateRepresentation::forStatement(const ASTree* _for){
    std::unique_ptr<IRTree> _irFor = std::make_unique<IRTree>(IRNodeType::FOR);
    _irFor->pushChild(assignmentStatement(_for->getChild(0)));
    _irFor->pushChild(relationalExpression(_for->getChild(1)));
    _irFor->pushChild(assignmentStatement(_for->getChild(2)));
    _irFor->pushChild(construct(_for->getChild(3)));

    return _irFor;
}

std::unique_ptr<IRTree> IntermediateRepresentation::doWhileStatement(const ASTree* _dowhile){
    std::unique_ptr<IRTree> _irDowhile = std::make_unique<IRTree>(IRNodeType::DO_WHILE);
    _irDowhile->pushChild(construct(_dowhile->getChild(0)));
    _irDowhile->pushChild(relationalExpression(_dowhile->getChild(1)));

    return _irDowhile;
}

std::unique_ptr<IRTree> IntermediateRepresentation::switchStatement(const ASTree* _switch){
    std::unique_ptr<IRTree> _irSwitch = std::make_unique<IRTree>(IRNodeType::SWITCH);
    _irSwitch->pushChild(id(_switch->getChild(0)));
    for(size_t i = 1; i < _switch->getChildren().size(); i++){
        ASTree* _astCase = _switch->getChild(i);
        if(_astCase->getNodeType() == ASTNodeType::CASE){
            _irSwitch->pushChild(_case(_astCase));
        }
        else{
            _irSwitch->pushChild(_default(_astCase));
        }
    }
    return _irSwitch;
}

std::unique_ptr<IRTree> IntermediateRepresentation::_case(const ASTree* _astCase){
    std::unique_ptr<IRTree> _irCase = std::make_unique<IRTree>(IRNodeType::CASE);
    _irCase->pushChild(literal(_astCase->getChild(0)));
    for(const auto& _construct : _astCase->getChild(1)->getChildren()){
        _irCase->pushChild(construct(_construct.get()));
        // ignore all constructs (including break) after return statement
        if(_irCase->getChildren().back()->getNodeType() == IRNodeType::RETURN){
            return _irCase;
        }
    }
    if(_astCase->getChildren().size() == 3){
        _irCase->pushChild(_break());
    }
    return _irCase;
}

std::unique_ptr<IRTree> IntermediateRepresentation::_default(const ASTree* _astDefault){
    std::unique_ptr<IRTree> _irDefault = std::make_unique<IRTree>(IRNodeType::DEFAULT);
    for(const auto& _construct : _astDefault->getChild(0)->getChildren()){
        _irDefault->pushChild(construct(_construct.get()));
        // ignore all constructs (including break) after return statement
        if(_irDefault->getChildren().back()->getNodeType() == IRNodeType::RETURN){
            break;
        }
    }
    return _irDefault;
}

std::unique_ptr<IRTree> IntermediateRepresentation::_break() const {
    return std::make_unique<IRTree>(IRNodeType::BREAK);
}

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRTree> IntermediateRepresentation::numericalExpression(const ASTree* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        return id(_numexp);
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        return literal(_numexp);
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCall(_numexp);
    }
    if(_numexp->getChildren().size() == 1){
        return numericalExpression(_numexp->getChild(0));
    }
    else{
        std::unique_ptr<IRTree> leftOperand{ numericalExpression(_numexp->getChild(0)) };
        std::unique_ptr<IRTree> rightOperand{ numericalExpression(_numexp->getChild(1)) };

        if(leftOperand->getNodeType() == rightOperand->getNodeType() && leftOperand->getNodeType() == IRNodeType::LITERAL){
            if(leftOperand->getType() == Types::INT){
                return mergeLiterals<int>(std::move(leftOperand), std::move(rightOperand), _numexp);
            }
            else if(leftOperand->getType() == Types::UNSIGNED){
                return mergeLiterals<unsigned>(std::move(leftOperand), std::move(rightOperand), _numexp);
            }
        }
        
        std::string val{ _numexp->getToken().value };
        Types type{ leftOperand->getType() };
        IRNodeType iNodeType{ arithmeticOperators.find(val) != arithmeticOperators.end() ? stringToArop.at(val) : stringToBitop.at(val)[type == Types::UNSIGNED] };
        std::unique_ptr<IRTree> _irNumexp = std::make_unique<IRTree>(iNodeType);
        
        _irNumexp->setType(type);
        _irNumexp->pushChild(std::move(leftOperand));
        _irNumexp->pushChild(std::move(rightOperand));
        return _irNumexp;
    }
}

// reducing the depth of the tree if both children are literals
template<typename T>
std::unique_ptr<IRTree> IntermediateRepresentation::mergeLiterals(std::unique_ptr<IRTree>&& leftOperand, std::unique_ptr<IRTree>&& rightOperand, const ASTree* _numexp) const {
    T lval = (std::is_same<T, int>::value ? std::stoi(leftOperand->getValue()) : std::stoul(leftOperand->getValue()));
    T rval = (std::is_same<T, int>::value ? std::stoi(rightOperand->getValue()) : std::stoul(rightOperand->getValue()));
    T result{ mergeValues<T>(lval, rval, _numexp) };

    Types type{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };
    std::string suffix{ type == Types::INT ? "" : "u" };

    return std::make_unique<IRTree>("", std::to_string(result) + suffix, type, IRNodeType::LITERAL);
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, const ASTree* _numexp) const {
    std::string op{ _numexp->getToken().value };
    if(op == "+") 
        return l + r;
    else if(op == "-") 
        return l - r;
    else if(op == "*") 
        return l * r;
    else if(op == "/"){
        if(r == 0){
            irContext.errors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> division by ZERO",
                    _numexp->getToken().line, _numexp->getToken().column)
            );
            return 0;
        }
        return l / r;
    }
    else if(op == "&") 
        return l & r;
    else if(op == "|") 
        return l | r;
    else if(op == "^") 
        return l ^ r;
    else if(op == "<<")
        return l << r;
    else if(op == ">>")
        return l >> r;
    else{
        // unreachable
        irContext.errors.push_back(
            std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid arithmetic operator '{}'",
                _numexp->getToken().line, _numexp->getToken().column, op)
        );
        return 0;
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::relationalExpression(const ASTree* _relexp){
    std::unique_ptr<IRTree> _irRelexp = std::make_unique<IRTree>(IRNodeType::CMP);
    _irRelexp->setValue(_relexp->getToken().value); // stores relational operator as value

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(_relexp->getChild(0)) };
    if(temps){
        _irRelexp->pushChild(std::move(temps));
    }
    temps = initiateTemporaries(_relexp->getChild(1));
    if(temps){
        _irRelexp->pushChild(std::move(temps));
    }

    _irRelexp->pushChild(numericalExpression(_relexp->getChild(0)));
    _irRelexp->pushChild(numericalExpression(_relexp->getChild(1)));

    return _irRelexp;
}

std::unique_ptr<IRTree> IntermediateRepresentation::id(const ASTree* _id) const {
    return std::make_unique<IRTree>(_id->getToken().value, "0", _id->getType(), IRNodeType::ID);
}

std::unique_ptr<IRTree> IntermediateRepresentation::literal(const ASTree* _literal) const {
    return std::make_unique<IRTree>("", _literal->getToken().value, _literal->getType(), IRNodeType::LITERAL);
}

std::unique_ptr<IRTree> IntermediateRepresentation::functionCall(const ASTree* _functionCall){
    std::unique_ptr<IRTree> _irFunctionCall = 
        std::make_unique<IRTree>(_functionCall->getToken().value, "", _functionCall->getType(), IRNodeType::CALL);
    _irFunctionCall->pushChild(argument(_functionCall));
    return _irFunctionCall;
}

std::unique_ptr<IRTree> IntermediateRepresentation::argument(const ASTree* _functionCall){
    std::unique_ptr<IRTree> _irArgument = std::make_unique<IRTree>(IRNodeType::ARGUMENT);
    for(const auto& _argument : _functionCall->getChild(0)->getChildren()){
        auto tmps = initiateTemporaries(_argument.get());
        if(tmps){
            _irArgument->pushChild(std::move(tmps));
        }
    }
    for(const auto& _argument : _functionCall->getChild(0)->getChildren()){
        _irArgument->pushChild(numericalExpression(_argument.get()));
    }
    return _irArgument;
}

// counting the number of function calls that should be replaced by temporary variables
size_t IntermediateRepresentation::countTemporaries(const ASTree* _numexp) const {
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::ID || _numexp->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else if(_numexp->getChildren().size() == 1){
        return countTemporaries(_numexp->getChild(0));
    }
    else{
        return countTemporaries(_numexp->getChild(0)) + countTemporaries(_numexp->getChild(1));
    }
}

// generating temporary variables
std::unique_ptr<IRTree> IntermediateRepresentation::generateTemporaries(){
    std::string name = std::format("_t{}", ++irContext.temporaries);
    irContext.temporaryNames.push(name);
    return std::make_unique<IRTree>(name, "0", Types::NO_TYPE, IRNodeType::VARIABLE);
}

// assigning a returned value to temporary variables
void IntermediateRepresentation::assignFunctionCalls(IRTree* _temporaryRoot, const ASTree* _numexp, size_t& idx){
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        auto temporary = _temporaryRoot->getChild(idx);
        temporary->pushChild(std::make_unique<IRTree>(IRNodeType::ASSIGN));
        temporary->getChild(0)->pushChild(std::make_unique<IRTree>(temporary->getName(), "0", _numexp->getType(), IRNodeType::ID));
        temporary->getChild(0)->pushChild(functionCall(_numexp));
        _temporaryRoot->setType(_numexp->getType());
        ++idx;
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::VARIABLE || _numexp->getNodeType() == ASTNodeType::ID){
        return;
    }
    else if(_numexp->getChildren().size() == 1){
        assignFunctionCalls(_temporaryRoot, _numexp->getChild(0), idx);
    }
    else{
        assignFunctionCalls(_temporaryRoot, _numexp->getChild(0), idx);
        assignFunctionCalls(_temporaryRoot, _numexp->getChild(1), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRTree> IntermediateRepresentation::replaceFunctionCall(const ASTree* _functionCall){
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRTree>(name, "0", _functionCall->getType(), IRNodeType::ID);
}

std::unique_ptr<IRTree> IntermediateRepresentation::initiateTemporaries(const ASTree* _numexp){
    size_t tmpCount = countTemporaries(_numexp);
    if(tmpCount > 0){
        std::unique_ptr<IRTree> temporaryRoot = std::make_unique<IRTree>(IRNodeType::TEMPORARY);
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->pushChild(generateTemporaries());
        }
        assignFunctionCalls(temporaryRoot.get(), _numexp, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}