#include "../analyzer.hpp"

#include <latch>
#include <memory>
#include <thread>
#include <cassert>
#include <iostream>

#include "../../thread-pool/thread_pool.hpp"
#include "../function_analyzer.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager) 
    : globalScopeManager{ scopeManager }, functionAnalyzer(scopeManager, semanticErrors, globalError) {}

void Analyzer::semanticCheck(const ASTProgram* _program){
    // global scope
    globalScopeManager.pushScope();

    // define all functions
    functionAnalyzer.checkFunctionSignatures(_program);

    // check if function signatures produced any errors
    if(hasSemanticError(_program)){
        return;
    }

    // concurrent analysis of functions
    startFunctionCheck(_program);

    globalScopeManager.popScope();
}

bool Analyzer::hasSemanticError(const ASTProgram* _program) const noexcept {
    for(const auto& _function : _program->getFunctions()){
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

void Analyzer::showSemanticErrors(const ASTProgram* _program) const {
    std::string err = "\nSemantic check: failed!\n";
    for(const auto& _function : _program->getFunctions()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors[funcName].empty()){
            for(const auto& exception : semanticErrors[funcName]){
                err += std::format("{}\n", exception);
            }
        }
    }
    if(!semanticErrors[globalError].empty()){
        for(const auto& exception : semanticErrors[globalError]){
            err += std::format("{}\n", exception);
        }
    }
    std::cerr << err;
}

void Analyzer::startFunctionCheck(const ASTProgram* _program){
    // handling threads for concurrent function analysis
    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(_program->getFunctionCount()) };

    for(const auto& _function : _program->getFunctions()){
        threadPool.enqueue([&, _function=_function.get()] {
            functionAnalyzer.checkFunction(_function);
            doneLatch.count_down();
        });
    }

    doneLatch.wait();
}