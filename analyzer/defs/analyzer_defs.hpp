#ifndef ANALYZER_DEFS_HPP
#define ANALYZER_DEFS_HPP

#include <string>

#include "../../symbol-handling/scope-manager/scope_manager.hpp"

/*
    context for semantic analysis threads
*/

struct AnalyzerThreadContext {
    // name of a function analyzed by the thread
    std::string functionName = "";
    // function scope manager
    ScopeManager* scopeManager = nullptr;
    // buffer for exception messages
    std::vector<std::string> semanticErrors;

    void init(std::string_view funcName, ScopeManager* scopeMng){
        functionName = funcName;
        scopeManager = scopeMng;
    }

    void reset(){
        functionName = "";
        scopeManager = nullptr;
        semanticErrors.clear();
    }
};

#endif