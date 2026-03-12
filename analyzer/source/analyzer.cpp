#include "../analyzer.hpp"

#include <latch>
#include <memory>
#include <sstream>
#include <thread>
#include <cassert>

#include "../../thread-pool/thread_pool.hpp"
#include "../function_analyzer.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager) 
    : globalScopeManager{ scopeManager }, functionAnalyzer{ scopeManager, semanticErrors, globalError }, 
        directiveAnalyzer{ semanticErrors, globalError } {

    semanticErrors[globalError] = {};
}

void Analyzer::semanticCheck(const ASTProgram* program){
    // global scope
    globalScopeManager.pushScope();

    // check directives
    directiveAnalyzer.checkDir(program->getDirs());

    // define all functions
    functionAnalyzer.checkFunctionSignatures(program);

    // check if function signatures produced any errors
    if(hasSemanticError(program)){
        return;
    }

    // concurrent analysis of functions
    startFunctionCheck(program);

    globalScopeManager.popScope();
}

bool Analyzer::hasSemanticError(const ASTProgram* program) const noexcept {
    for(const auto& _function : program->getFunctions()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors[funcName].empty()){
            return true;
        }   
    }
    
    if(!semanticErrors[globalError].empty()){
        return true;
    }

    return false;
}

std::string Analyzer::getSemanticErrors(const ASTProgram* program) const noexcept {
    if(semanticErrors.empty()){
        return "";
    }

    std::stringstream errors{"Semantic check failed:\n"};
    size_t errLen = errors.str().length();

    for(const auto& _function : program->getFunctions()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors[funcName].empty()){
            for(const auto& error : semanticErrors[funcName]){
                errors << error << "\n";
            }
        }
    }
    if(!semanticErrors[globalError].empty()){
        for(const auto& error : semanticErrors[globalError]){
            errors << error << "\n";
        }
    }

    std::string strErrors = errors.str(); 

    return strErrors.length() != errLen ? strErrors : "";
}

void Analyzer::startFunctionCheck(const ASTProgram* program){
    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& _function : program->getFunctions()){
        threadPool.enqueue([&, _function=_function.get()] {
            functionAnalyzer.checkFunction(_function);
            doneLatch.count_down();
        });
    }

    doneLatch.wait();
}