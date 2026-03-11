#include "../function_analyzer.hpp"

#include <cassert>
#include <format>
#include <memory>

#include "../../common/abstract-syntax-tree/ast_parameter.hpp"
#include "../../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../../common/abstract-syntax-tree/ast_compound_stmt.hpp"

FunctionAnalyzer::FunctionAnalyzer(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err)
    : globalScopeManager{ scopeManager }, statementAnalyzer{ scopeManager }, semanticErrors{ semErrors }, globalError{ err } {}

thread_local AnalyzerThreadContext FunctionAnalyzer::analyzerContext;

AnalyzerThreadContext& FunctionAnalyzer::getContext() noexcept {
    return analyzerContext;
}

void FunctionAnalyzer::checkFunctionSignatures(const ASTProgram* program){
    for(const auto& function : program->getFunctions()){
        Type returnType{ function->getType() };
        const std::string& funcName = function->getToken().value;
        semanticErrors[funcName] = {};

        // function redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{funcName, Kind::FUN, returnType})){
            semanticErrors[globalError].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                    function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
            );
            return;
        }

        // function type check
        if(returnType == Type::NO_TYPE){
            semanticErrors[funcName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
            );
        }
        else if(returnType == Type::AUTO){
            semanticErrors[funcName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                    function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
            );
        }
        globalScopeManager.pushScope();
        checkParameters(function.get());
        globalScopeManager.popScope();
    }

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kind::FUN})){
        semanticErrors[globalError].push_back("'main' function not found");
    }
}

void FunctionAnalyzer::checkFunction(const ASTFunction* function){
    // initializing thread context
    SymbolTable symTable;
    ScopeManager functionScopeManager{symTable};
    analyzerContext.init(function->getToken().value, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(function);
    if(!function->isPredefined()){
        checkBody(function);
    }
    analyzerContext.scopeManager->popScope();
    
    // function return type check
    if(function->getType() != Type::VOID && !alwaysReturns(function)){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' not all paths return value", 
                function->getToken().line, function->getToken().column, typeToString.at(function->getType()), function->getToken().value)
        );
    }

    {
        std::lock_guard<std::mutex> lock(exceptionMtx);
        assert(semanticErrors[analyzerContext.functionName].empty());
        semanticErrors[analyzerContext.functionName] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void FunctionAnalyzer::checkParameters(const ASTFunction* function){
    const std::string& functionName = function->getToken().value;
    const auto& parameters = function->getParameters();

    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(functionName).setParameters(&parameters);
    
    // parameter check for main
    if(functionName == "main" && parameters.size() > 0){
        semanticErrors[functionName].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
                parameters[0]->getToken().line, parameters[0]->getToken().column)
        );
    }

    for(const auto& parameter : parameters){
        // parameter type check
        Type type{ parameter->getType() };
        if(type == Type::VOID || type == Type::NO_TYPE){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                    parameter->getToken().line, parameter->getToken().column, typeToString.at(type), parameter->getToken().value)
            );
        }
        else if(type == Type::AUTO){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                    parameter->getToken().line, parameter->getToken().column, typeToString.at(type), parameter->getToken().value)
            );
        }

        // parameter redefinition check
        if(!globalScopeManager.pushSymbol(Symbol{parameter->getToken().value, Kind::PAR, parameter->getType()})){
            semanticErrors[functionName].push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                    parameter->getToken().line, parameter->getToken().column, parameter->getToken().value)
            );
        }
    }
}

void FunctionAnalyzer::defineParameters(const ASTFunction* function){
    for(const auto& parameter : function->getParameters()){
        analyzerContext.scopeManager->pushSymbol(Symbol{parameter->getToken().value, Kind::PAR, parameter->getType()});
    }
}

void FunctionAnalyzer::checkBody(const ASTFunction* function){
    for(const auto& stmt : function->getBody()){
        statementAnalyzer.checkStmt(stmt.get());
    }
}

bool FunctionAnalyzer::alwaysReturns(const ASTNode* node) const noexcept {
    ASTNodeType nodeType{ node->getNodeType() };
    if(nodeType == ASTNodeType::RETURN_STATEMENT){
        return true;
    }
    else if(nodeType == ASTNodeType::FUNCTION){
        auto function = static_cast<const ASTFunction*>(node);
        for(const auto& stmt : function->getBody()){
            // if a statement of a function always returns => function always returns
            if(alwaysReturns(stmt.get())){
                return true;
            }
        }
        return false;
    }
    else if(nodeType == ASTNodeType::IF_STATEMENT){
        auto ifStmt = static_cast<const ASTIfStmt*>(node);
        // if there is no else, not all paths are covered
        if(!ifStmt->hasElse()){
            return false;
        }
        // if any if / else if / else doesn't return => not all paths return
        for(const auto& stmt : ifStmt->getStatements()){
            if(!alwaysReturns(stmt.get())){
                return false;
            }
        }
        return true;
    }
    else if(nodeType == ASTNodeType::SWITCH_STATEMENT){
        auto switchStmt = static_cast<const ASTSwitchStmt*>(node);
        // no default => not all paths are covered
        if(!switchStmt->hasDefault()){
            return false;
        }
        // each case must return, otherwise not all paths return
        for(const auto& caseStmt : switchStmt->getCases()){
            // check cases, if there is no break fallthrough to the next case
            if(!alwaysReturns(caseStmt->getSwitchBlock()) && caseStmt->hasBreak()){
                return false;
            }
        }
        return alwaysReturns(switchStmt->getDefault()->getSwitchBlock()); // check default
    }
    else if(nodeType == ASTNodeType::DO_WHILE_STATEMENT){
        auto dowhileStmt = static_cast<const ASTDoWhileStmt*>(node);
        return alwaysReturns(dowhileStmt->getStatement());
    }
    else if(nodeType == ASTNodeType::COMPOUND_STATEMENT){
        auto compoundStmt = static_cast<const ASTCompoundStmt*>(node);
        // if any statement always returns - compound always returns
        for(const auto& stmt : compoundStmt->getStatements()){
            if(alwaysReturns(stmt.get())){
                return true;
            }
        }
        return false;
    }
    else if(nodeType == ASTNodeType::SWITCH_BLOCK){
        auto switchBlockStmt = static_cast<const ASTSwitchBlockStmt*>(node);
        for(const auto& stmt : switchBlockStmt->getStatements()){
            if(alwaysReturns(stmt.get())){
                return true;
            }
        }
        return false;
    }
    // at the moment constant relational expressions aren't evaluated at the compile-time, so WHILE and FOR will return false by default
    return false;
}