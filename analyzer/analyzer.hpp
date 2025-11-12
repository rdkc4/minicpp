#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <string>
#include <vector>

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "function_analyzer.hpp"

/**
 * @class Analyzer
 * @brief semantic analysis of the abstract syntax tree
*/
class Analyzer{
public:
    Analyzer(ScopeManager& scopeManager);

    /** 
     * @brief entry point for the semantic check
     * @param _program - pointer to a root of the ast
     * @returns void
    */
    void semanticCheck(const ASTProgram* _program);

    /** 
     * @brief checks if code is semantically correct
     * @param _program - const pointer to a root of the ast
     * @note _program is needed to obtain name of all functions
     * @returns true if any semantic error is caught, false otherwise
    */
    bool hasSemanticError(const ASTProgram* _program) const noexcept;

    /** 
     * @brief displays semantic errors
     * @param _program - const pointer to a root of the ast
     * @note _program is needed to obtain name of all functions
     * @returns void
    */
    void showSemanticErrors(const ASTProgram* _program) const;

protected:
    ScopeManager& globalScopeManager;
    FunctionAnalyzer functionAnalyzer;

    mutable std::unordered_map<std::string, std::vector<std::string>> semanticErrors;
    
    /// identifier for signature errors
    static constexpr std::string globalError{ "__global" };

    /** 
     * @brief starts checking functions parallelly
     * @param _program - const pointer to a root of the ast
     * @returns void
    */
    void startFunctionCheck(const ASTProgram* _program);
};

#endif
