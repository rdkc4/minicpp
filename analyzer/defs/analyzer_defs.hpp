#ifndef ANALYZER_DEFS_HPP
#define ANALYZER_DEFS_HPP

#include <string>

#include "../../symbol-handling/scope-manager/scope_manager.hpp"

/** 
 * @struct AnalyzerThreadContext
 * @brief context of the thread checking semantics of the function
*/
struct AnalyzerThreadContext {
    /// name of the function analyzed by the thread
    std::string functionName = "";
    /// pointer to the function scope manager
    ScopeManager* scopeManager = nullptr;
    /// vector for exception messages
    std::vector<std::string> semanticErrors;

    /** 
     * @brief initializes the context of the thread
     * @param funcName - name of the function
     * @param scopeMng - pointer to the scope manager
     * @returns void
    */
    void init(std::string_view funcName, ScopeManager* scopeMng){
        functionName = funcName;
        scopeManager = scopeMng;
    }

    /** 
     * @brief resets the context of the thread
     * @returns void
    */
    void reset(){
        functionName = "";
        scopeManager = nullptr;
        semanticErrors.clear();
    }
};

#endif