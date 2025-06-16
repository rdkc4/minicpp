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

#include "../thread-pool/thread_pool.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager } {}

thread_local AnalyzerThreadContext Analyzer::analyzerContext;

void Analyzer::semanticCheck(const ASTree* root){
    ASTree* _functionList{ root->getChildren().back().get() };

    // global scope
    globalScopeManager.pushScope();

    // define all functions
    for(const auto& _function : _functionList->getChildren()){
        checkFunctionSignatures(_function.get());
    }

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }

    // concurrent analysis of functions
    startFunctionCheck(_functionList);

    // throw all errors in order
    checkSemanticErrors(_functionList);

    globalScopeManager.popScope();
}

void Analyzer::checkSemanticErrors(const ASTree* _functionList) const {
    std::string errors{""};
    for(const auto& _function : _functionList->getChildren()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors[funcName].empty()){
            for(const auto& err : semanticErrors[funcName]){
                errors += err + "\n";
            }
        }
    }
    if(!errors.empty()){
        throw std::runtime_error(errors);
    }
}

void Analyzer::checkFunctionSignatures(const ASTree* _function){
    Types returnType{ _function->getType() };
    const std::string& funcName = _function->getToken().value; 
    semanticErrors[funcName] = {};

    // function type check
    if(returnType == Types::NO_TYPE){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
        );
    }
    else if(returnType == Types::AUTO){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
        );
    }

    // function redefinition check
    if(!globalScopeManager.pushSymbol(Symbol{funcName, Kinds::FUN, returnType})){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                _function->getToken().line, _function->getToken().column, typeToString.at(returnType), _function->getToken().value)
        );
    }

    globalScopeManager.pushScope();
    checkParameter(_function->getChild(0), funcName);
    globalScopeManager.popScope();
}

void Analyzer::startFunctionCheck(const ASTree* _functionList){
    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    // counter of analyzed functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    size_t total = _functionList->getChildren().size();

    for(const auto& _function : _functionList->getChildren()){
        threadPool.enqueue([&, _function=_function.get()] {
            checkFunction(_function);

            // semantic analysis of a function ended
            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_all();
            }
        });
    }

    {
        // wait until each function is analyzed
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

}

void Analyzer::checkFunction(const ASTree* _function){
    // initializing thread context
    SymbolTable symTable;
    ScopeManager functionScopeManager{symTable};
    analyzerContext.init(_function->getToken().value, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(_function->getChild(0));
    checkBody(_function->getChild(1));
    analyzerContext.scopeManager->popScope();
    
    // function return type check
    if(_function->getType() != Types::VOID && !analyzerContext.returned){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' returns '{}'", 
                _function->getToken().line, _function->getToken().column, typeToString.at(_function->getType()), _function->getToken().value, typeToString.at(Types::VOID))
        );
    }

    {
        std::lock_guard<std::mutex> lock(exceptionMtx);
        assert(semanticErrors[analyzerContext.functionName].empty());
        semanticErrors[analyzerContext.functionName] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void Analyzer::checkParameter(ASTree* _parameters, const std::string& functionName){
    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(functionName).setParameters(_parameters);
    
    // parameter check for main
    if(functionName == "main" && _parameters->getChildren().size() > 0){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
                _parameters->getToken().line, _parameters->getToken().column)
        );
    }

    for(const auto& _parameter : _parameters->getChildren()){
        // parameter type check
        Types type{ _parameter->getType() };
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    _parameter->getToken().line, _parameter->getToken().column, typeToString.at(type), _parameter->getToken().value)
            );
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                    _parameter->getToken().line, _parameter->getToken().column, typeToString.at(type), _parameter->getToken().value)
            );
        }

        // parameter redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{_parameter->getToken().value, Kinds::PAR, _parameter->getType()})){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                    _parameter->getToken().line, _parameter->getToken().column, _parameter->getToken().value)
            );
        }
    }
}

void Analyzer::defineParameters(const ASTree* _parameters){
    for(const auto& _parameter : _parameters->getChildren()){
        analyzerContext.scopeManager->pushSymbol(Symbol{_parameter->getToken().value, Kinds::PAR, _parameter->getType()});
    }
}

void Analyzer::checkBody(const ASTree* _body){
    for(const auto& _construct : _body->getChildren()){
        checkConstruct(_construct.get());
    }
}

void Analyzer::checkConstruct(const ASTree* _construct){
    if(_construct->getNodeType() == ASTNodeType::VARIABLE){
        checkVariable(_construct);
    }
    else{
        checkStatement(_construct);
    }
}

void Analyzer::checkVariable(const ASTree* _variable){
    // variable type check
    Types type{ _variable->getType() };
    if(type == Types::VOID || type == Types::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }
    else if(type == Types::AUTO && _variable->getChildren().empty()){
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
    if(_variable->getChildren().size() != 0){
        checkAssignmentStatement(_variable->getChild(0));
    }
}

void Analyzer::checkStatement(const ASTree* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::PRINTF:
            checkPrintfStatement(_statement);
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(_statement);
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(_statement);
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(_statement);
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(_statement);
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(_statement);
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(_statement);
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(_statement);
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(_statement);
            break;
        default:
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                    _statement->getToken().line, _statement->getToken().column, astNodeTypeToString.at(_statement->getNodeType()))
            );
    }
}

// child(0) - numerical expression that should be printed
void Analyzer::checkPrintfStatement(const ASTree* _printf){
    checkNumericalExpression(_printf->getChild(0));
}

// child(0) - if relational expression 
// child 1 - if statement constructs
// child(i), i > 1 && i % 2 == 0 relational expression
// child(i), i > 1 && i % 2 == 1 constructs
// if there is odd number of children, else statement is at the back
void Analyzer::checkIfStatement(const ASTree* _if){
    for(const auto& child : _if->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            checkRelationalExpression(child.get());
        }
        else{
            checkConstruct(child.get());
        }
    }
}

// child(0) - relational expression
// child(1) - constructs
void Analyzer::checkWhileStatement(const ASTree* _while){
    checkRelationalExpression(_while->getChild(0));
    checkConstruct(_while->getChild(1));
}

// child(0) - assignment statement (initializer)
// child(1) - relational expression (condition)
// child(2) - assignment statement (increment)
// child(3) - constructs
void Analyzer::checkForStatement(const ASTree* _for){
    checkAssignmentStatement(_for->getChild(0));
    checkRelationalExpression(_for->getChild(1));
    checkAssignmentStatement(_for->getChild(2));

    std::string initializerName{ _for->getChild(0)->getChild(0)->getToken().value };
    std::string incrementName{ _for->getChild(2)->getChild(0)->getToken().value };

    // check if variables initializer variable and increment variable match
    if(initializerName != incrementName){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid for statement - variable mismatch '{}' != '{}'", 
                _for->getToken().line, _for->getToken().column, initializerName, incrementName)
        );
    }

    checkConstruct(_for->getChild(3));
}

// child(0) - constructs
// child(1) - relational expression
void Analyzer::checkDoWhileStatement(const ASTree* _dowhile){
    checkConstruct(_dowhile->getChild(0));
    checkRelationalExpression(_dowhile->getChild(1));
}

// child(0) - id
// child(1) - cases and default
void Analyzer::checkSwitchStatement(const ASTree* _switch){
    // if id is not defined, switch is ignored, all cases will be ignored, including their constructs 
    if(!checkID(_switch->getChild(0))) return;
    
    // case check
    if(_switch->getChild(0)->getType() == Types::INT){
        checkSwitchStatementCases<int>(_switch);
    }
    else if(_switch->getChild(0)->getType() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(_switch);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                _switch->getToken().line, _switch->getToken().column, _switch->getChild(0)->getToken().value)
        );
    }
}

// cases child(0) - literal
// cases child(1) - constructs
// cases child(2) - break (optional)
// default child(0) - constructs (break can exist, but it will be ignored)
template<typename T>
void Analyzer::checkSwitchStatementCases(const ASTree* _switch){
    std::unordered_set<T> set;
    Types expectedType{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };

    for(size_t i = 1; i < _switch->getChildren().size(); ++i){
        ASTree* _case{ _switch->getChild(i) };
        if(_case->getNodeType() == ASTNodeType::CASE){
            checkLiteral(_case->getChild(0));

            T val;
            // case type check
            Types type{ _case->getChild(0)->getType() };
            if(_case->getChild(0)->getType() != expectedType){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                        _case->getToken().line, _case->getToken().column, typeToString.at(expectedType), typeToString.at(type))
                );
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(_case->getChild(0)->getToken().value) 
                                                                : std::stoul(_case->getChild(0)->getToken().value))) != set.end()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                        _case->getToken().line, _case->getToken().column, _case->getChild(0)->getToken().value)
                );
            }
            else{
                for(const auto& _construct : _case->getChild(1)->getChildren()){
                    checkConstruct(_construct.get());
                }
                set.insert(val);
            }
        }
        else{
            // default case
            for(const auto& _construct : _case->getChild(0)->getChildren()){
                checkConstruct(_construct.get());
            }
        }
    }
}

void Analyzer::checkCompoundStatement(const ASTree* _compound){
    // compound scope
    analyzerContext.scopeManager->pushScope();
    for(const auto& _construct : _compound->getChildren()){
        checkConstruct(_construct.get());
    }
    analyzerContext.scopeManager->popScope();
}

// child(0) - destination variable
// child 1 - numerical expression
void Analyzer::checkAssignmentStatement(const ASTree* _assignment){
    ASTree* _variable{ _assignment->getChild(0) };
    ASTree* _value{ _assignment->getChild(1) };

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

void Analyzer::checkReturnStatement(const ASTree* _return){
    Types returnType{ _return->getChildren().empty() ? Types::VOID : getNumericalExpressionType(_return->getChild(0)) };
    
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
    analyzerContext.returned = true;
}

void Analyzer::checkNumericalExpression(ASTree* _numexp){
    // setting type to numexp node for easier type checking
    _numexp->setType(getNumericalExpressionType(_numexp));
}

Types Analyzer::getNumericalExpressionType(ASTree* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        checkID(_numexp);
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        checkLiteral(_numexp);
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(_numexp);
        return _numexp->getType();
    }
    if(_numexp->getChildren().size() == 1){
        return getNumericalExpressionType(_numexp->getChild(0));
    }else{
        // numerical expression type check
        Types ltype{ getNumericalExpressionType(_numexp->getChild(0)) };
        Types rtype{ getNumericalExpressionType(_numexp->getChild(1)) };

        // if either subexpression is invalid, root expression shall be invalid too, no point checking for type mismatch
        if(ltype == Types::NO_TYPE || rtype == Types::NO_TYPE) return Types::NO_TYPE;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                    _numexp->getToken().line, _numexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
            );
        }
        
        return ltype;
    }
}

// node - contains relational operator
// child(0) - left operand
// child(1) - right operand
void Analyzer::checkRelationalExpression(const ASTree* _relexp){
    ASTree* leftOperand{ _relexp->getChild(0) };
    ASTree* rightOperand{ _relexp->getChild(1) };

    checkNumericalExpression(leftOperand);
    Types ltype{ leftOperand->getType() };
    checkNumericalExpression(rightOperand);
    Types rtype{ rightOperand->getType() };
    
    // undefined element, no point checking for type mismatch
    if(ltype == Types::NO_TYPE || rtype == Types::NO_TYPE) return;

    // relational expression type check
    if(ltype != rtype){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type mismatch: expected '{}', got '{}'", 
                _relexp->getToken().line, _relexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
}

bool Analyzer::checkID(ASTree* _id){
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

void Analyzer::checkLiteral(const ASTree* _literal) const {
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
void Analyzer::checkFunctionCall(ASTree* _functionCall){
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
    size_t expectedParams{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters()->getChildren().size() };
    if(_functionCall->getChild(0)->getChildren().size() != expectedParams){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided arguments '{}', expected '{}'", 
                _functionCall->getToken().line, _functionCall->getToken().column, _functionCall->getToken().value, 
                _functionCall->getChild(0)->getChildren().size(), expectedParams)
        );
        _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
        // number of provided arguments differs from expected, no type checking
        return;
    }
    _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
    checkArgument(_functionCall);
}

void Analyzer::checkArgument(const ASTree* _functionCall){
    ASTree* functionParameters{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters() };
    ASTree* _arguments{ _functionCall->getChild(0) };

    size_t i{ 0 };
    for(const auto& _argument : _arguments->getChildren()){
        checkNumericalExpression(_argument.get());

        // type check of corresponding parameters in function and function call
        Types ltype{ _argument->getType() };
        Types rtype{ functionParameters->getChild(i)->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Types::NO_TYPE) return;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    _argument->getToken().line, _argument->getToken().column, typeToString.at(rtype), typeToString.at(ltype), _argument->getToken().value)
            );
        }
        ++i;
    }
}