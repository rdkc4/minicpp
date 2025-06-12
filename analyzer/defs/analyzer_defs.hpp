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
    // whether function returned or not
    bool returned = false;
    // function scope manager
    ScopeManager* scopeManager = nullptr;
    // buffer for exception messages
    std::vector<std::string> semanticErrors;

    void init(std::string_view funcName, ScopeManager* scopeMng){
        functionName = funcName;
        returned = false;
        scopeManager = scopeMng;
    }

    void reset(){
        functionName = "";
        returned = false;
        scopeManager = nullptr;
        semanticErrors.clear();
    }
};

#endif