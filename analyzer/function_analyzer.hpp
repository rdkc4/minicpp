#ifndef FUNCTION_ANALYZER_HPP
#define FUNCTION_ANALYZER_HPP

#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTParameter.hpp"
#include "defs/analyzer_defs.hpp"
#include "statement_analyzer.hpp"

/** 
 * @class FunctionAnalyzer
 * @brief semantic analysis of the AST function
*/
class FunctionAnalyzer {
public:
    FunctionAnalyzer(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err);

    /** 
     * @brief semantic check for the signature of all functions of the program
     * @param _program - const pointer to a root of the ast
     * @returns void 
    */
    void checkFunctionSignatures(const ASTProgram* _program);

    /** 
     * @brief semantic check of the single function
     * @param _function - const pointer to a function
     * @returns void
    */
    void checkFunction(const ASTFunction* _function);

    /** 
     * @brief getter for the context of the function that is analyzed by the current thread
     * @returns reference to a context of the analyzer thread
    */
    static AnalyzerThreadContext& getContext() noexcept;

protected:
    /// thread local context of the analyzer
    static thread_local AnalyzerThreadContext analyzerContext;

    /// reference to the global scope manager
    ScopeManager& globalScopeManager;

    /// mutex for the concurrent access to semantic errors map
    mutable std::mutex exceptionMtx;

    /// maps function name to its semantic errors
    std::unordered_map<std::string, std::vector<std::string>>& semanticErrors;

    /// label of the global scope errors
    const std::string& globalError;

    /// analyzer specialized for statements
    StatementAnalyzer statementAnalyzer;

    /** 
     * @brief semantic check for the parameters of the current function
     * @param _parameters - const reference to a vector of pointers to parameters
     * @param functionName - name of the function that is currently being analyzed 
     * @returns void
    */
    void checkParameter(const std::vector<std::unique_ptr<ASTParameter>>& _parameters, const std::string& functionName);

    /** 
     * @brief inserts parameters of the function into the symbol table
     * @param _parameters - const reference to a vector of pointers to parameters
     * @returns void
    */
    void defineParameters(const std::vector<std::unique_ptr<ASTParameter>>& _parameters);

    /**
     * @brief semantic check for the body of the function
     * @param _body - const reference to a vector of pointers to statements inside of the body
     * @returns void
    */
    void checkBody(const std::vector<std::unique_ptr<ASTStatement>>& _body);

    /** 
     * @brief check if the non-void function always returns
     * @param _construct - const pointer to any node of the ast
     * @returns true if function always returns, false otherwise
    */
    bool alwaysReturns(const ASTNode* _construct) const noexcept;
};

#endif