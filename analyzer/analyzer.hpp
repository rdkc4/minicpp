#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"
#include "function_analyzer.hpp"

/*
    Semantic analysis of abstract syntax tree
*/
class Analyzer{
    public:
        
        Analyzer(ScopeManager& scopeManager);

        void semanticCheck(const ASTProgram* _program);

        static AnalyzerThreadContext& getThreadContext() noexcept;

    protected:
        // detection of undefined/redefined variables/functions, type checking
        ScopeManager& globalScopeManager;
        FunctionAnalyzer functionAnalyzer;

        mutable std::mutex exceptionMtx;
        mutable std::unordered_map<std::string, std::vector<std::string>> semanticErrors;
        
        static constexpr std::string globalError{ "__global" };
        
        void checkSemanticErrors(const ASTProgram* _functions) const;

        // function
        void startFunctionCheck(const ASTProgram* _program);
};

#endif
