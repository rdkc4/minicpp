#ifndef FUNCTION_ANALYZER_HPP
#define FUNCTION_ANALYZER_HPP

#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>

#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "defs/analyzer_defs.hpp"
#include "statement_analyzer.hpp"

/** 
 * @class FunctionAnalyzer
 * @brief semantic analysis of the AST function
*/
class FunctionAnalyzer {
public:
    /** 
     * @brief Creates the instance of the analyzer specialized for functions
     * @param scopeManager - reference to the global scope manager
     * @param semErrors - reference to the map connecting function names with their semantic errors
     * @param err - global error label
    */
    FunctionAnalyzer(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err);

    /** 
     * @brief semantic check for the signature of all functions of the program
     * @param program - const pointer to a root of the ast
    */
    void checkFunctionSignatures(const ASTProgram* program);

    /** 
     * @brief semantic check of the single function
     * @param function - const pointer to a function
    */
    void checkFunction(const ASTFunction* function);

    /** 
     * @brief semantic check for the parameters of the current function
     * @param function - const pointer to the ast function
    */
    void checkParameters(const ASTFunction* function);

    /** 
     * @brief inserts parameters of the function into the symbol table
     * @param function - const pointer to the ast function
    */
    void defineParameters(const ASTFunction* function);

    /**
     * @brief semantic check for the body of the function
     * @param function - const pointer to the ast function
    */
    void checkBody(const ASTFunction* function);

    /** 
     * @brief check if the non-void function always returns
     * @param node - const pointer to any node of the ast
     * @returns true if function always returns, false otherwise
    */
    bool alwaysReturns(const ASTNode* node) const noexcept;

    /** 
     * @brief getter for the context of the function that is analyzed by the current thread
     * @returns reference to a context of the analyzer thread
    */
    static AnalyzerThreadContext& getContext() noexcept;

private:
    /// thread local context of the analyzer
    static thread_local AnalyzerThreadContext analyzerContext;

    /// reference to the global scope manager
    ScopeManager& globalScopeManager;

    /// mutex for the concurrent access to semantic errors map
    mutable std::mutex exceptionMtx;

    /// analyzer specialized for statements
    StatementAnalyzer statementAnalyzer;

protected:
    /// maps function name to its semantic errors
    std::unordered_map<std::string, std::vector<std::string>>& semanticErrors;

    /// label of the global scope errors
    const std::string globalError;

};

#endif