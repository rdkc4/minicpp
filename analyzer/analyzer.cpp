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
    ASTree* flist{ root->getChildren().back().get() };

    // global scope
    globalScopeManager.pushScope();

    // define all functions
    for(const auto& child : flist->getChildren()){
        checkFunctionSignatures(child.get());
    }

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }

    // concurrent analysis of functions
    startFunctionCheck(flist);

    // throw all errors in order
    checkSemanticErrors(flist);

    globalScopeManager.popScope();
}

void Analyzer::checkSemanticErrors(const ASTree* node) const {
    std::string errors{""};
    for(const auto& child : node->getChildren()){
        const std::string& funcName = child->getToken().value;
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

void Analyzer::checkFunctionSignatures(const ASTree* node){
    Types returnType{ node->getType() };
    const std::string& funcName = node->getToken().value; 
    semanticErrors[funcName] = {};

    // function type check
    if(returnType == Types::NO_TYPE){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value)
        );
    }
    else if(returnType == Types::AUTO){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value)
        );
    }

    // function redefinition check
    if(!globalScopeManager.pushSymbol(Symbol{funcName, Kinds::FUN, returnType})){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value)
        );
    }

    globalScopeManager.pushScope();
    checkParameter(node->getChild(0), funcName);
    globalScopeManager.popScope();
}

void Analyzer::startFunctionCheck(const ASTree* flist){
    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    // counter of analyzed functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    size_t total = flist->getChildren().size();

    for(const auto& child : flist->getChildren()){
        threadPool.enqueue([&, child=child.get()] {
            checkFunction(child);

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

void Analyzer::checkFunction(const ASTree* node){
    // initializing thread context
    SymbolTable symTable;
    ScopeManager functionScopeManager{symTable};
    analyzerContext.init(node->getToken().value, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(node->getChild(0));
    checkBody(node->getChild(1));
    analyzerContext.scopeManager->popScope();
    
    // function return type check
    if(node->getType() != Types::VOID && !analyzerContext.returned){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' returns '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(node->getType()), node->getToken().value, typeToString.at(Types::VOID))
        );
    }

    {
        std::lock_guard<std::mutex> lock(exceptionMtx);
        assert(semanticErrors[analyzerContext.functionName].empty());
        semanticErrors[analyzerContext.functionName] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void Analyzer::checkParameter(ASTree* node, const std::string& functionName){
    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(functionName).setParameters(node);
    
    // parameter check for main
    if(functionName == "main" && node->getChildren().size() > 0){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
                node->getToken().line, node->getToken().column)
        );
    }

    for(const auto& child : node->getChildren()){
        // parameter type check
        Types type{ child->getType() };
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value)
            );
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                    child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value)
            );
        }

        // parameter redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{child->getToken().value, Kinds::PAR, child->getType()})){
            throw std::runtime_error(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                    child->getToken().line, child->getToken().column, child->getToken().value)
            );
        }
    }
}

void Analyzer::defineParameters(ASTree* node){
    for(const auto& child : node->getChildren()){
        analyzerContext.scopeManager->pushSymbol(Symbol{child->getToken().value, Kinds::PAR, child->getType()});
    }
}

void Analyzer::checkBody(const ASTree* node){
    for(const auto& child : node->getChildren()){
        checkConstruct(child.get());
    }
}

void Analyzer::checkConstruct(const ASTree* node){
    if(node->getNodeType() == ASTNodeType::VARIABLE){
        checkVariable(node);
    }
    else{
        checkStatement(node);
    }
}

void Analyzer::checkVariable(const ASTree* node){
    // variable type check
    Types type{ node->getType() };
    if(type == Types::VOID || type == Types::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value)
        );
    }
    else if(type == Types::AUTO && node->getChildren().empty()){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
                node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value)
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{node->getToken().value, Kinds::VAR, type})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                node->getToken().line, node->getToken().column, node->getToken().value)
        );
    }

    // direct initialization
    if(node->getChildren().size() != 0){
        checkStatement(node->getChild(0));
    }
}

void Analyzer::checkStatement(const ASTree* node){
    switch(node->getNodeType()){
        case ASTNodeType::PRINTF:
            checkPrintfStatement(node);
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(node);
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(node);
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(node);
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(node);
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(node);
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(node);
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(node);
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(node);
            break;
        default:
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                    node->getToken().line, node->getToken().column, astNodeTypeToString.at(node->getNodeType()))
            );
    }
}

// child(0) - numerical expression that should be printed
void Analyzer::checkPrintfStatement(const ASTree* node){
    checkNumericalExpression(node->getChild(0));
}

// child(0) - if relational expression 
// child 1 - if statement constructs
// child(i), i > 1 && i % 2 == 0 relational expression
// child(i), i > 1 && i % 2 == 1 constructs
// if there is odd number of children, else statement is at the back
void Analyzer::checkIfStatement(const ASTree* node){
    for(const auto& child : node->getChildren()){
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
void Analyzer::checkWhileStatement(const ASTree* node){
    checkRelationalExpression(node->getChild(0));
    checkConstruct(node->getChild(1));
}

// child(0) - assignment statement (initializer)
// child(1) - relational expression (condition)
// child(2) - assignment statement (increment)
// child(3) - constructs
void Analyzer::checkForStatement(const ASTree* node){
    checkAssignmentStatement(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
    checkAssignmentStatement(node->getChild(2));

    std::string lname{ node->getChild(0)->getChild(0)->getToken().value };
    std::string rname{ node->getChild(2)->getChild(0)->getToken().value };

    // check if variables initializer variable and increment variable match
    if(lname != rname){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid for statement - variable mismatch '{}' != '{}'", 
                node->getToken().line, node->getToken().column, lname, rname)
        );
    }

    checkConstruct(node->getChild(3));
}

// child(0) - constructs
// child(1) - relational expression
void Analyzer::checkDoWhileStatement(const ASTree* node){
    checkConstruct(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
}

// child(0) - id
// child(1) - cases and default
void Analyzer::checkSwitchStatement(const ASTree* node){
    // if id is not defined, switch is ignored, all cases will be ignored, including their constructs 
    if(!checkID(node->getChild(0))) return;
    
    // case check
    if(node->getChild(0)->getType() == Types::INT){
        checkSwitchStatementCases<int>(node);
    }
    else if(node->getChild(0)->getType() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(node);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                node->getToken().line, node->getToken().column, node->getChild(0)->getToken().value)
        );
    }
}

// cases child(0) - literal
// cases child(1) - constructs
// cases child(2) - break (optional)
// default child(0) - constructs (break can exist, but it will be ignored)
template<typename T>
void Analyzer::checkSwitchStatementCases(const ASTree* node){
    std::unordered_set<T> set;
    Types expectedType{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };

    for(size_t i = 1; i < node->getChildren().size(); i++){
        ASTree* child{ node->getChild(i) };
        if(child->getNodeType() == ASTNodeType::CASE){
            checkLiteral(child->getChild(0));

            T val;
            // case type check
            Types type{ child->getChild(0)->getType() };
            if(child->getChild(0)->getType() != expectedType){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                        child->getToken().line, child->getToken().column, typeToString.at(expectedType), typeToString.at(type))
                );
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(child->getChild(0)->getToken().value) 
                                                                : std::stoul(child->getChild(0)->getToken().value))) != set.end()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                        child->getToken().line, child->getToken().column, child->getChild(0)->getToken().value)
                );
            }
            else{
                for(const auto& _child : child->getChild(1)->getChildren()){
                    checkConstruct(_child.get());
                }
                set.insert(val);
            }
        }
        else{
            for(const auto& _child : child->getChild(0)->getChildren()){
                checkConstruct(_child.get());
            }
        }
    }
}

void Analyzer::checkCompoundStatement(const ASTree* node){
    // compound scope
    analyzerContext.scopeManager->pushScope();
    for(const auto& child : node->getChildren()){
        checkConstruct(child.get());
    }
    analyzerContext.scopeManager->popScope();
}

// child(0) - destination variable
// child 1 - numerical expression
void Analyzer::checkAssignmentStatement(const ASTree* node){
    ASTree* lchild{ node->getChild(0) };
    ASTree* rchild{ node->getChild(1) };

    // if variable is not defined, rvalue won't be analyzed
    if(!checkID(lchild)) return;

    checkNumericalExpression(rchild);
    Types rtype{ rchild->getType() };
    Types ltype{ lchild->getType() };

    // if numexp contains undefined element it will report the undefined variable, no point checking for type mismatch
    if(rtype == Types::NO_TYPE) return;

    // assignment type check
    if(rtype != ltype && ltype != Types::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }

    if(ltype == Types::AUTO){
        analyzerContext.scopeManager->getSymbol(lchild->getToken().value).setType(rtype);
    }
}

void Analyzer::checkReturnStatement(const ASTree* node){
    Types type{ node->getChildren().empty() ? Types::VOID : getNumericalExpressionType(node->getChild(0)) };
    
    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(type == Types::NO_TYPE) return;
    
    // return type check
    Types returnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != type){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(returnType), analyzerContext.functionName, typeToString.at(type))
        );
    }
    analyzerContext.returned = true;
}

void Analyzer::checkNumericalExpression(ASTree* node){
    // setting type to numexp node for easier type checking
    node->setType(getNumericalExpressionType(node));
}

Types Analyzer::getNumericalExpressionType(ASTree* node){
    if(node->getNodeType() == ASTNodeType::ID){
        checkID(node);
        return node->getType();
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL){
        checkLiteral(node);
        return node->getType();
    }
    else if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(node);
        return node->getType();
    }
    if(node->getChildren().size() == 1){
        return getNumericalExpressionType(node->getChild(0));
    }else{
        // numerical expression type check
        Types ltype{ getNumericalExpressionType(node->getChild(0)) };
        Types rtype{ getNumericalExpressionType(node->getChild(1)) };

        // if either subexpression is invalid, root expression shall be invalid too, no point checking for type mismatch
        if(ltype == Types::NO_TYPE || rtype == Types::NO_TYPE) return Types::NO_TYPE;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                    node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
            );
        }
        
        return ltype;
    }
}

// node - contains relational operator
// child(0) - left operand
// child(1) - right operand
void Analyzer::checkRelationalExpression(const ASTree* node){
    ASTree* lchild{ node->getChild(0) };
    ASTree* rchild{ node->getChild(1) };

    checkNumericalExpression(lchild);
    Types ltype{ lchild->getType() };
    checkNumericalExpression(rchild);
    Types rtype{ rchild->getType() };
    
    // undefined element, no point checking for type mismatch
    if(ltype == Types::NO_TYPE || rtype == Types::NO_TYPE) return;

    // relational expression type check
    if(ltype != rtype){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type mismatch: expected '{}', got '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
}

bool Analyzer::checkID(ASTree* node){
    std::string name{ node->getToken().value };
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                node->getToken().line, node->getToken().column, name)
        );
        node->setType(Types::NO_TYPE);
        return false;
    }
    node->setType(analyzerContext.scopeManager->getSymbol(name).getType());
    return true;
}

void Analyzer::checkLiteral(const ASTree* node) const {
    std::string name{ node->getToken().value };
    // invalid literal check
    if((node->getType() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (node->getType() == Types::INT && name.back() == 'u')){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
                node->getToken().line, node->getToken().column, name)
        );
    }
}

// child(0) - arguments
void Analyzer::checkFunctionCall(ASTree* node){
    // function existence
    if(!globalScopeManager.lookupSymbol(node->getToken().value, {Kinds::FUN})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
                node->getToken().line, node->getToken().column, node->getToken().value)
        );
        // function doesn't exist, no point checking arguments
        return;
    }
    
    // main function can't be called
    if(node->getToken().value == "main"){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
                node->getToken().line, node->getToken().column)
        );
        return;
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams{ globalScopeManager.getSymbol(node->getToken().value).getParameters()->getChildren().size() };
    if(node->getChild(0)->getChildren().size() != expectedParams){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided arguments '{}', expected '{}'", 
                node->getToken().line, node->getToken().column, node->getToken().value, node->getChild(0)->getChildren().size(), expectedParams)
        );
        node->setType(globalScopeManager.getSymbol(node->getToken().value).getType());
        // number of provided arguments differs from expected, no type checking
        return;
    }
    node->setType(globalScopeManager.getSymbol(node->getToken().value).getType());
    checkArgument(node);
}

void Analyzer::checkArgument(const ASTree* node){
    ASTree* functionParameters{ globalScopeManager.getSymbol(node->getToken().value).getParameters() };
    ASTree* arguments{ node->getChild(0) };

    size_t i{ 0 };
    for(const auto& child : arguments->getChildren()){
        checkNumericalExpression(child.get());

        // type check of corresponding parameters in function and function call
        Types ltype{ child->getType() };
        Types rtype{ functionParameters->getChild(i)->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Types::NO_TYPE) return;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    child->getToken().line, child->getToken().column, typeToString.at(rtype), typeToString.at(ltype), child->getToken().value)
            );
        }
        ++i;
    }
}