#include "../function_analyzer.hpp"

#include <cassert>
#include <format>

#include "../../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../../common/abstract-syntax-tree/ASTCompoundSt.hpp"

FunctionAnalyzer::FunctionAnalyzer(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err)
    : globalScopeManager{ scopeManager }, semanticErrors{ semErrors }, globalError{ err }, statementAnalyzer{ scopeManager } {}

thread_local AnalyzerThreadContext FunctionAnalyzer::analyzerContext;

AnalyzerThreadContext& FunctionAnalyzer::getContext() noexcept {
    return analyzerContext;
}

void FunctionAnalyzer::checkFunctionSignatures(const ASTProgram* _program){
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

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kinds::FUN})){
        semanticErrors[globalError].push_back("'main' function not found");
    }
}

void FunctionAnalyzer::checkFunction(const ASTFunction* _function){
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

void FunctionAnalyzer::checkParameter(const std::vector<std::unique_ptr<ASTParameter>>& _parameters, const std::string& functionName){
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

void FunctionAnalyzer::defineParameters(const std::vector<std::unique_ptr<ASTParameter>>& _parameters){
    for(const auto& _parameter : _parameters){
        analyzerContext.scopeManager->pushSymbol(Symbol{_parameter->getToken().value, Kinds::PAR, _parameter->getType()});
    }
}

void FunctionAnalyzer::checkBody(const std::vector<std::unique_ptr<ASTStatement>>& _body){
    for(const auto& _statement : _body){
        statementAnalyzer.checkStatement(_statement.get());
    }
}

bool FunctionAnalyzer::alwaysReturns(const ASTNode* _construct) const noexcept {
    ASTNodeType nodeType{ _construct->getNodeType() };
    if(nodeType == ASTNodeType::RETURN_STATEMENT){
        return true;
    }
    else if(nodeType == ASTNodeType::FUNCTION){
        auto _function = static_cast<const ASTFunction*>(_construct);
        for(const auto& _statement : _function->getBody()){
            // if a statement of a function always returns => function always returns
            if(alwaysReturns(_statement.get())){
                return true;
            }
        }
        return false;
    }
    else if(nodeType == ASTNodeType::IF_STATEMENT){
        auto _ifSt = static_cast<const ASTIfSt*>(_construct);
        // if there is no else, not all paths are covered
        if(!_ifSt->hasElse()){
            return false;
        }
        // if any if / else if / else doesn't return => not all paths return
        for(const auto& _statement : _ifSt->getStatements()){
            if(!alwaysReturns(_statement.get())){
                return false;
            }
        }
        return true;
    }
    else if(nodeType == ASTNodeType::SWITCH_STATEMENT){
        auto _switch = static_cast<const ASTSwitchSt*>(_construct);
        // no default => not all paths are covered
        if(!_switch->hasDefault()){
            return false;
        }
        // each case must return, otherwise not all paths return
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
        // if any statement always returns - compound always returns
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