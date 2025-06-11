#include "analyzer.hpp"
#include "defs/analyzer_defs.hpp"

#include <atomic>
#include <condition_variable>
#include <exception>
#include <memory>
#include <stdexcept>
#include <format>
#include <thread>
#include <mutex>
#include <vector>
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

    globalScopeManager.popScope();
}

void Analyzer::checkFunctionSignatures(const ASTree* node){
    Types returnType{ node->getType() };

    // function type check
    if(returnType == Types::NO_TYPE){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }
    else if(returnType == Types::AUTO){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }

    // function redefinition check
    if(!globalScopeManager.pushSymbol(Symbol{node->getToken().value, Kinds::FUN, returnType})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }
    globalScopeManager.pushScope();
    checkParameter(node->getChild(0), node->getToken().value);
    globalScopeManager.popScope();
}

void Analyzer::startFunctionCheck(const ASTree* flist){
    std::vector<std::string> exceptions;
    std::mutex exceptionMtx;

    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    // counter of analyzed functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    size_t total = flist->getChildren().size();

    for(const auto& child : flist->getChildren()){
        threadPool.enqueue([&, child=child.get()] {
            try {
                checkFunction(child);
            } catch (std::exception& ex) {
                std::lock_guard<std::mutex> lock(exceptionMtx);
                exceptions.push_back(ex.what());
            }

            // semantic analysis of a function ended
            done.fetch_add(1);
            if(done == total){
                doneCv.notify_all();
            }
        });
    }

    {
        // wait until each function is analyzed
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

    if (!exceptions.empty()) {
        std::string errorMessage{ "Semantic errors:\n" };
        for(const auto& ex : exceptions){
            errorMessage += ex + "\n";
        }
        throw std::runtime_error(errorMessage);
    }
}

void Analyzer::checkFunction(const ASTree* node){
    // initializing thread context
    SymbolTable symTable;
    ScopeManager functionScopeManager{symTable};
    analyzerContext.functionName = node->getToken().value;
    analyzerContext.returned = false;
    analyzerContext.scopeManager = &functionScopeManager;

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(node->getChild(0));
    checkBody(node->getChild(1));
    analyzerContext.scopeManager->popScope();
    
    analyzerContext.scopeManager = nullptr;

    // function return type check
    if(node->getType() != Types::VOID && !analyzerContext.returned){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' returns '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(node->getType()), node->getToken().value, typeToString.at(Types::VOID)));
    }
}

void Analyzer::checkParameter(ASTree* node, const std::string& functionName){
    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(functionName).setParameters(node);
    
    // parameter check for main
    if(functionName == "main" && node->getChildren().size() > 0){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
            node->getToken().line, node->getToken().column));
    }

    for(const auto& child : node->getChildren()){
        // parameter type check
        Types type{ child->getType() };
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value));
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value));
        }

        // parameter redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{child->getToken().value, Kinds::PAR, child->getType()})){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                child->getToken().line, child->getToken().column, child->getToken().value));
        }
    }
}

void Analyzer::defineParameters(ASTree* node) const {
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
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value));
    }
    else if(type == Types::AUTO && node->getChildren().empty()){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value));
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{node->getToken().value, Kinds::VAR, type})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value));
    }

    // direct initialization
    if(node->getChildren().size() != 0){
        checkStatement(node->getChild(0));
    }
}

void Analyzer::checkStatement(const ASTree* node){
    switch(node->getNodeType()){
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
            throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                node->getToken().line, node->getToken().column, astNodeTypeToString.at(node->getNodeType())));
    }
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
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid for statement - variable mismatch '{}' != '{}'", 
            node->getToken().line, node->getToken().column, lname, rname));
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
    checkID(node->getChild(0));

    // case check
    if(node->getChild(0)->getType() == Types::INT){
        checkSwitchStatementCases<int>(node);
    }
    else if(node->getChild(0)->getType() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(node);
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
            node->getToken().line, node->getToken().column, node->getChild(0)->getToken().value));
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
                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                    child->getToken().line, child->getToken().column, typeToString.at(expectedType), typeToString.at(type)));
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(child->getChild(0)->getToken().value) 
                                                                : std::stoul(child->getChild(0)->getToken().value))) != set.end()){

                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                    child->getToken().line, child->getToken().column, child->getChild(0)->getToken().value));
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
void Analyzer::checkAssignmentStatement(const ASTree* node) const {
    ASTree* lchild{ node->getChild(0) };
    ASTree* rchild{ node->getChild(1) };
    
    checkNumericalExpression(rchild);
    Types rtype{ rchild->getType() };
    
    checkID(lchild);
    Types ltype{ analyzerContext.scopeManager->getSymbol(lchild->getToken().value).getType() };
    
    // assignment type check
    if(rtype != ltype && ltype != Types::AUTO){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
    }

    if(ltype == Types::AUTO){
        analyzerContext.scopeManager->getSymbol(lchild->getToken().value).setType(rtype);
    }
}

void Analyzer::checkReturnStatement(const ASTree* node){
    Types type{ node->getChildren().empty() ? Types::VOID : getNumericalExpressionType(node->getChild(0)) };

    // return type check
    Types returnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != type){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), analyzerContext.functionName, typeToString.at(type)));
    }
    analyzerContext.returned = true;
}

void Analyzer::checkNumericalExpression(ASTree* node) const {
    // setting type to numexp node for easier type checking
    node->setType(getNumericalExpressionType(node));
}

Types Analyzer::getNumericalExpressionType(ASTree* node) const {
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
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
        }
        
        return ltype;
    }
}

// node - contains relational operator
// child(0) - left operand
// child(1) - right operand
void Analyzer::checkRelationalExpression(const ASTree* node) const {
    ASTree* lchild{ node->getChild(0) };
    ASTree* rchild{ node->getChild(1) };

    checkNumericalExpression(lchild);
    Types ltype{ lchild->getType() };
    checkNumericalExpression(rchild);
    Types rtype{ rchild->getType() };
    
    // relational expression type check
    if(ltype != rtype){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type mismatch: expected '{}', got '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
    }
}

void Analyzer::checkID(ASTree* node) const {
    std::string name{ node->getToken().value };
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
            node->getToken().line, node->getToken().column, name));
    }
    node->setType(analyzerContext.scopeManager->getSymbol(name).getType());
}

void Analyzer::checkLiteral(const ASTree* node) const {
    std::string name{ node->getToken().value };
    // invalid literal check
    if((node->getType() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (node->getType() == Types::INT && name.back() == 'u')){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
            node->getToken().line, node->getToken().column, name));
    }
}

// child(0) - arguments
void Analyzer::checkFunctionCall(ASTree* node) const {
    // function existence
    if(!globalScopeManager.lookupSymbol(node->getToken().value, {Kinds::FUN})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value));
    }
    
    // check if main is called
    if(node->getToken().value == "main"){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
            node->getToken().line, node->getToken().column));
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams{ globalScopeManager.getSymbol(node->getToken().value).getParameters()->getChildren().size() };
    if(node->getChild(0)->getChildren().size() != expectedParams){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided parameters '{}', expected '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value, node->getChild(0)->getChildren().size(), expectedParams));
    }
    node->setType(globalScopeManager.getSymbol(node->getToken().value).getType());
    checkArgument(node);
}

void Analyzer::checkArgument(const ASTree* node) const {
    ASTree* functionParameters{ globalScopeManager.getSymbol(node->getToken().value).getParameters() };
    ASTree* arguments{ node->getChild(0) };

    size_t i{ 0 };
    for(const auto& child : arguments->getChildren()){
        checkNumericalExpression(child.get());

        // type check of corresponding parameters in function and function call
        Types ltype{ child->getType() };
        Types rtype{ functionParameters->getChild(i)->getType() };
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                child->getToken().line, child->getToken().column, typeToString.at(rtype), typeToString.at(ltype), child->getToken().value));
        }
        ++i;
    }
}