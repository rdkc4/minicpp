#include "../analyzer.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <stdexcept>
#include <thread>
#include <cassert>

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
    checkSemanticErrors(_program);

    // concurrent analysis of functions
    startFunctionCheck(_program);

    // throw all errors in order
    checkSemanticErrors(_program);

    globalScopeManager.popScope();
}

// merging errors captured in each function and reporting it back if there are any
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
            functionAnalyzer.checkFunction(_function);

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