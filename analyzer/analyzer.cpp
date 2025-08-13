#include "analyzer.hpp"
#include "defs/analyzer_defs.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <stdexcept>
#include <format>
#include <thread>
#include <mutex>
#include <vector>
#include <cassert>

#include "../common/abstract-syntax-tree/ASTBinaryExpression.hpp"
#include "../thread-pool/thread_pool.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager } {}

thread_local AnalyzerThreadContext Analyzer::analyzerContext;

void Analyzer::semanticCheck(const ASTProgram* _program){
    // global scope
    globalScopeManager.pushScope();

    // define all functions
    checkFunctionSignatures(_program);

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kinds::FUN})){
        semanticErrors[globalError].push_back("'main' function not found");
    }

    // check if function signatures produced any errors
    checkSemanticErrors(_program);

    // concurrent analysis of functions
    startFunctionCheck(_program);

    // throw all errors in order
    checkSemanticErrors(_program);

    globalScopeManager.popScope();
}

void Analyzer::checkSemanticErrors(const ASTProgram* _program) const {
    std::string errors{""};
    for(const auto& _function : _program->getFunctions()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors[funcName].empty()){
            for(const auto& err : semanticErrors[funcName]){
                errors += err + "\n";
            }
        }
    }
    if(!semanticErrors[globalError].empty()){
        for(const auto& err : semanticErrors[globalError]){
            errors += err + "\n";
        }
    }
    if(!errors.empty()){
        throw std::runtime_error(errors);
    }
}

void Analyzer::checkFunctionSignatures(const ASTProgram* _program){
    semanticErrors[globalError] = {};
    for(const auto& _function : _program->getFunctions()){
        Types returnType{ _function->getType() };
        const std::string& funcName = _function->getToken().value;
        semanticErrors[funcName] = {};

        // function redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{funcName, Kinds::FUN, returnType})){
            semanticErrors[globalError].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                    _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
            );
            return;
        }

        // function type check
        if(returnType == Types::NO_TYPE){
            semanticErrors[funcName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
            );
        }
        else if(returnType == Types::AUTO){
            semanticErrors[funcName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                    _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
            );
        }
        globalScopeManager.pushScope();
        checkParameter(_function->getParameters(), funcName);
        globalScopeManager.popScope();
    }
}

void Analyzer::startFunctionCheck(const ASTProgram* _program){
    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    // counter of analyzed functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    size_t total = _program->getFunctionCount();

    for(const auto& _function : _program->getFunctions()){
        threadPool.enqueue([&, _function=_function.get()] {
            checkFunction(_function);

            // semantic analysis of a function ended
            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_one();
            }
        });
    }

    {
        // wait until each function is analyzed
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

}

void Analyzer::checkFunction(const ASTFunction* _function){
    // initializing thread context
    SymbolTable symTable;
    ScopeManager functionScopeManager{symTable};
    analyzerContext.init(_function->getToken().value, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(_function->getParameters());
    checkBody(_function->getBody());
    analyzerContext.scopeManager->popScope();
    
    // function return type check
    if(_function->getType() != Types::VOID && !alwaysReturns(_function)){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' not all paths return value", 
                _function->getToken().line, _function->getToken().column, typeToString.at(_function->getType()), _function->getToken().value)
        );
    }

    {
        std::lock_guard<std::mutex> lock(exceptionMtx);
        assert(semanticErrors[analyzerContext.functionName].empty());
        semanticErrors[analyzerContext.functionName] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void Analyzer::checkParameter(const std::vector<std::unique_ptr<ASTParameter>>& _parameters, const std::string& functionName){
    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(functionName).setParameters(&_parameters);
    
    // parameter check for main
    if(functionName == "main" && _parameters.size() > 0){
        semanticErrors[functionName].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
                _parameters[0]->getToken().line, _parameters[0]->getToken().column)
        );
    }

    for(const auto& _parameter : _parameters){
        // parameter type check
        Types type{ _parameter->getType() };
        if(type == Types::VOID || type == Types::NO_TYPE){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    _parameter->getToken().line, _parameter->getToken().column, typeToString.at(type), _parameter->getToken().value)
            );
        }
        else if(type == Types::AUTO){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                    _parameter->getToken().line, _parameter->getToken().column, typeToString.at(type), _parameter->getToken().value)
            );
        }

        // parameter redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{_parameter->getToken().value, Kinds::PAR, _parameter->getType()})){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                    _parameter->getToken().line, _parameter->getToken().column, _parameter->getToken().value)
            );
        }
    }
}

void Analyzer::defineParameters(const std::vector<std::unique_ptr<ASTParameter>>& _parameters){
    for(const auto& _parameter : _parameters){
        analyzerContext.scopeManager->pushSymbol(Symbol{_parameter->getToken().value, Kinds::PAR, _parameter->getType()});
    }
}

void Analyzer::checkBody(const std::vector<std::unique_ptr<ASTStatement>>& _body){
    for(const auto& _statement : _body){
        checkStatement(_statement.get());
    }
}

void Analyzer::checkStatement(const ASTStatement* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::VARIABLE:
            checkVariable(static_cast<const ASTVariable*>(_statement));
            break;
        case ASTNodeType::PRINTF:
            checkPrintfStatement(static_cast<const ASTPrintfSt*>(_statement));
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(static_cast<const ASTIfSt*>(_statement));
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(static_cast<const ASTWhileSt*>(_statement));
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(static_cast<const ASTForSt*>(_statement));
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(static_cast<const ASTAssignSt*>(_statement));
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(static_cast<const ASTCompoundSt*>(_statement));
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(static_cast<const ASTReturnSt*>(_statement));
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(static_cast<const ASTDoWhileSt*>(_statement));
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(static_cast<const ASTSwitchSt*>(_statement));
            break;
        default:
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                    _statement->getToken().line, _statement->getToken().column, astNodeTypeToString.at(_statement->getNodeType()))
            );
    }
}

void Analyzer::checkVariable(const ASTVariable* _variable){
    // variable type check
    Types type{ _variable->getType() };
    if(type == Types::VOID || type == Types::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }
    else if(type == Types::AUTO && !_variable->hasAssign()){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{_variable->getToken().value, Kinds::VAR, type})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                _variable->getToken().line, _variable->getToken().column, _variable->getToken().value)
        );
    }

    // direct initialization
    if(_variable->getAssign() != nullptr){
        checkNumericalExpression(_variable->getAssignNC());

        Types rtype{_variable->getAssign()->getType() };

        // variable initialization type check
        if(rtype != type && type != Types::AUTO){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                    _variable->getToken().line, _variable->getToken().column, typeToString.at(type), typeToString.at(rtype))
            );
        }

        if(type == Types::AUTO){
            analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
        }
    }
}

// child(0) - numerical expression that should be printed
void Analyzer::checkPrintfStatement(const ASTPrintfSt* _printf){
    checkNumericalExpression(_printf->getExpNC());
}

// child(0) - if relational expression 
// child 1 - if statement constructs
// child(i), i > 1 && i % 2 == 0 relational expression
// child(i), i > 1 && i % 2 == 1 constructs
// if there is odd number of children, else statement is at the back
void Analyzer::checkIfStatement(const ASTIfSt* _if){
    for(const auto& _condition : _if->getConditions()){
        checkNumericalExpression(_condition.get());
    }
    for(const auto& _statement : _if->getStatements()){
        checkStatement(_statement.get());
    }
}

// child(0) - relational expression
// child(1) - constructs
void Analyzer::checkWhileStatement(const ASTWhileSt* _while){
    checkNumericalExpression(_while->getConditionNC());
    checkStatement(_while->getStatement());
}

// child(0) - assignment statement (initializer)
// child(1) - relational expression (condition)
// child(2) - assignment statement (increment)
// child(3) - constructs
void Analyzer::checkForStatement(const ASTForSt* _for){
    if(_for->hasInitializer()){
        checkAssignmentStatement(_for->getInitializer());
    }
    if(_for->hasCondition()){
        checkNumericalExpression(_for->getConditionNC());
    }
    if(_for->hasIncrementer()){
        checkAssignmentStatement(_for->getIncrementer());
    }

    checkStatement(_for->getStatement());
}

// child(0) - constructs
// child(1) - relational expression
void Analyzer::checkDoWhileStatement(const ASTDoWhileSt* _dowhile){
    checkStatement(_dowhile->getStatement());
    checkNumericalExpression(_dowhile->getConditionNC());
}

// child(0) - id
// child(1) - cases and default
void Analyzer::checkSwitchStatement(const ASTSwitchSt* _switch){
    // if id is not defined, switch is ignored, all cases will be ignored, including their constructs 
    if(!checkID(_switch->getVariableNC())) return;
    
    // case check
    if(_switch->getVariable()->getType() == Types::INT){
        checkSwitchStatementCases<int>(_switch);
    }
    else if(_switch->getVariable()->getType() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(_switch);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                _switch->getToken().line, _switch->getToken().column, _switch->getVariable()->getToken().value)
        );
    }
}

// cases child(0) - literal
// cases child(1) - constructs
// cases child(2) - break (optional)
// default child(0) - constructs (break can exist, but it will be ignored)
template<typename T>
void Analyzer::checkSwitchStatementCases(const ASTSwitchSt* _switch){
    std::unordered_set<T> set;
    Types expectedType{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };

    for(const auto& _case : _switch->getCases()){
        checkLiteral(_case->getLiteral());

        T val;
        // case type check
        Types type{ _case->getLiteral()->getType() };
        if(type != expectedType){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                    _case->getToken().line, _case->getToken().column, typeToString.at(expectedType), typeToString.at(type))
            );
        }
        // case duplicate check
        else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(_case->getLiteral()->getToken().value) 
                                                            : std::stoul(_case->getLiteral()->getToken().value))) != set.end()){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                    _case->getToken().line, _case->getToken().column, _case->getLiteral()->getToken().value)
            );
        }
        else{
            for(const auto& _statement : _case->getSwitchBlock()->getStatements()){
                checkStatement(_statement.get());
            }
            set.insert(val);
        }
    }
    if(_switch->hasDefault()){
        for(const auto& _statement : _switch->getDefault()->getSwitchBlock()->getStatements()){
            checkStatement(_statement.get());
        }
    }
}

void Analyzer::checkCompoundStatement(const ASTCompoundSt* _compound){
    // compound scope
    analyzerContext.scopeManager->pushScope();
    for(const auto& _statement : _compound->getStatements()){
        checkStatement(_statement.get());
    }
    analyzerContext.scopeManager->popScope();
}

// child(0) - destination variable
// child 1 - numerical expression
void Analyzer::checkAssignmentStatement(const ASTAssignSt* _assignment){
    ASTId* _variable{ _assignment->getVariableNC() };
    ASTExpression* _value{ _assignment->getExpNC() };

    // if variable is not defined, rvalue won't be analyzed
    if(!checkID(_variable)) return;

    checkNumericalExpression(_value);
    Types rtype{ _value->getType() };
    Types ltype{ _variable->getType() };

    // if numexp contains undefined element it will report the undefined variable, no point checking for type mismatch
    if(rtype == Types::NO_TYPE) return;

    // assignment type check
    if(rtype != ltype && ltype != Types::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                _assignment->getToken().line, _assignment->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }

    if(ltype == Types::AUTO){
        analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
    }
}

void Analyzer::checkReturnStatement(const ASTReturnSt* _return){
    Types returnType{ _return->returns() ? getNumericalExpressionType(_return->getExpNC()) : Types::VOID };
    
    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Types::NO_TYPE) return;
    
    // return type check
    Types expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                _return->getToken().line, _return->getToken().column, typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType))
        );
    }
}

void Analyzer::checkNumericalExpression(ASTExpression* _numexp){
    // setting type to numexp node for easier type checking
    _numexp->setType(getNumericalExpressionType(_numexp));
}

Types Analyzer::getNumericalExpressionType(ASTExpression* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        checkID(static_cast<ASTId*>(_numexp));
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        checkLiteral(static_cast<const ASTLiteral*>(_numexp));
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(static_cast<ASTFunctionCall*>(_numexp));
        return _numexp->getType();
    } // no unary operators yet
    else{
        // numerical expression type check
        ASTBinaryExpression* binExp = static_cast<ASTBinaryExpression*>(_numexp);
        Types ltype{ getNumericalExpressionType(binExp->getLeftOperandNC()) };
        Types rtype{ getNumericalExpressionType(binExp->getRightOperandNC()) };

        // if either subexpression is invalid, root expression shall be invalid too, no point checking for type mismatch
        if(ltype == Types::NO_TYPE || rtype == Types::NO_TYPE) return Types::NO_TYPE;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                    _numexp->getToken().line, _numexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
            );
            return Types::NO_TYPE;
        }
        
        return ltype;
    }
}

void Analyzer::checkRelationalExpression(ASTExpression* _relexp){
    ASTBinaryExpression* _binExp = static_cast<ASTBinaryExpression*>(_relexp);
    checkNumericalExpression(_binExp->getLeftOperandNC());
    checkNumericalExpression(_binExp->getRightOperandNC());
    
    auto ltype = _binExp->getLeftOperand()->getType();
    auto rtype = _binExp->getRightOperand()->getType();
    if(ltype != rtype){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                _relexp->getToken().line, _relexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
    _binExp->setType(ltype);
}

bool Analyzer::checkID(ASTId* _id){
    std::string name{ _id->getToken().value };
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                _id->getToken().line, _id->getToken().column, name)
        );
        _id->setType(Types::NO_TYPE);
        return false;
    }
    _id->setType(analyzerContext.scopeManager->getSymbol(name).getType());
    return true;
}

void Analyzer::checkLiteral(const ASTLiteral* _literal) const {
    std::string name{ _literal->getToken().value };
    // invalid literal check
    if((_literal->getType() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (_literal->getType() == Types::INT && name.back() == 'u')){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
                _literal->getToken().line, _literal->getToken().column, name)
        );
    }
}

// child(0) - arguments
void Analyzer::checkFunctionCall(ASTFunctionCall* _functionCall){
    // function existence
    if(!globalScopeManager.lookupSymbol(_functionCall->getToken().value, {Kinds::FUN})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
                _functionCall->getToken().line, _functionCall->getToken().column, _functionCall->getToken().value)
        );
        // function doesn't exist, no point checking arguments
        return;
    }
    
    // main function can't be called
    if(_functionCall->getToken().value == "main"){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
                _functionCall->getToken().line, _functionCall->getToken().column)
        );
        return;
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters()->size() };
    if(_functionCall->getArgumentCount() != expectedParams){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided arguments '{}', expected '{}'", 
                _functionCall->getToken().line, _functionCall->getToken().column, _functionCall->getToken().value, _functionCall->getArgumentCount(), expectedParams)
        );
        _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
        // number of provided arguments differs from expected, no type checking
        return;
    }
    _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
    checkArgument(_functionCall);
}

void Analyzer::checkArgument(const ASTFunctionCall* _functionCall){
    const std::vector<std::unique_ptr<ASTParameter>>* functionParameters{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters() };
    const std::vector<std::unique_ptr<ASTExpression>>& _arguments{ _functionCall->getArguments() };

    for(size_t i = 0; i < _functionCall->getArgumentCount(); ++i){
        checkNumericalExpression(_arguments[i].get());

        // type check of corresponding parameters in function and function call
        Types ltype{ _arguments[i]->getType() };
        Types rtype{ (*functionParameters)[i]->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Types::NO_TYPE) return;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    _arguments[i]->getToken().line, _arguments[i]->getToken().column, typeToString.at(rtype), typeToString.at(ltype), _arguments[i]->getToken().value)
            );
        }
    }
}

bool Analyzer::alwaysReturns(const ASTNode* _construct) const noexcept {
    ASTNodeType nodeType{ _construct->getNodeType() };
    if(nodeType == ASTNodeType::RETURN_STATEMENT){
        return true;
    }
    else if(nodeType == ASTNodeType::FUNCTION){
        auto _function = static_cast<const ASTFunction*>(_construct);
        for(const auto& _statement : _function->getBody()){
            if(alwaysReturns(_statement.get())){
                return true;
            }
        }
        return false;
    }
    else if(nodeType == ASTNodeType::IF_STATEMENT){
        auto _ifSt = static_cast<const ASTIfSt*>(_construct);
        if(!_ifSt->hasElse()){
            return false;
        }
        for(const auto& _statement : _ifSt->getStatements()){
            if(!alwaysReturns(_statement.get())){
                return false;
            }
        }
        return true;
    }
    else if(nodeType == ASTNodeType::SWITCH_STATEMENT){
        auto _switch = static_cast<const ASTSwitchSt*>(_construct);
        if(!_switch->hasDefault()){
            return false;
        }
        for(const auto& _case : _switch->getCases()){
            // check cases, if there is no break fallthrough to the next case
            if(!alwaysReturns(_case->getSwitchBlock()) && _case->hasBreak()){
                return false;
            }
        }
        return alwaysReturns(_switch->getDefault()->getSwitchBlock()); // check default
    }
    else if(nodeType == ASTNodeType::DO_WHILE_STATEMENT){
        auto _doWhileSt = static_cast<const ASTDoWhileSt*>(_construct);
        return alwaysReturns(_doWhileSt->getStatement());
    }
    else if(nodeType == ASTNodeType::COMPOUND_STATEMENT){
        auto _compoundSt = static_cast<const ASTCompoundSt*>(_construct);
        for(const auto& _statement : _compoundSt->getStatements()){
            if(alwaysReturns(_statement.get())){
                return true;
            }
        }
        return false;
    }
    else if(nodeType == ASTNodeType::SWITCH_BLOCK){
        auto _switchBlock = static_cast<const ASTSwitchBlock*>(_construct);
        for(const auto& _statement : _switchBlock->getStatements()){
            if(alwaysReturns(_statement.get())){
                return true;
            }
        }
        return false;
    }
    // at the moment constant relational expressions aren't evaluated at the compile-time, so WHILE and FOR will return false by default
    return false;
}