#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <string>
#include <vector>

#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "function_analyzer.hpp"
#include "directive_analyzer.hpp"

/**
 * @class Analyzer
 * @brief semantic analysis of the abstract syntax tree
*/
class Analyzer{
public:
    /** 
     * @brief Creates the instance of the analyzer
     * @param scopeManager - reference to the global scope manager
    */
    Analyzer(ScopeManager& scopeManager);

    /** 
     * @brief entry point for the semantic check
     * @param program - pointer to a root of the ast
    */
    void semanticCheck(const ASTProgram* program);

    /** 
     * @brief checks if code is semantically correct
     * @param program - const pointer to a root of the ast
     * @note _program is needed to obtain name of all functions
     * @returns true if any semantic error is caught, false otherwise
    */
    bool hasSemanticError(const ASTProgram* program) const noexcept;

    /** 
     * @brief getter for the semantic errors
     * @param program - const pointer to a root of the ast
     * @note _program is needed to obtain name of all functions
     * @returns semantic errors merged into a string 
    */
    std::string getSemanticErrors(const ASTProgram* program) const noexcept;

protected:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;
    
    /// analyzer specialized for functions
    FunctionAnalyzer functionAnalyzer;

    // analyzer specialized for directives
    DirectiveAnalyzer directiveAnalyzer;

    /// maps functionName to its semantic errors
    mutable std::unordered_map<std::string, std::vector<std::string>> semanticErrors;
    
    /// identifier for signature errors
    static inline std::string globalError{ "__global" };

    /** 
     * @brief starts checking functions parallelly
     * @param program - const pointer to a root of the ast
    */
    void startFunctionCheck(const ASTProgram* program);
};

#endif
