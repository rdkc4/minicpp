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
    // whether function returns or not
    bool returned = false;
    // function local scope manager
    ScopeManager* scopeManager = nullptr;
};

#endif