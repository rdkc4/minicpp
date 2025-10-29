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

class FunctionAnalyzer {
public:
    FunctionAnalyzer(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err);

    void checkFunctionSignatures(const ASTProgram* _program);
    void checkFunction(const ASTFunction* _function);

    static AnalyzerThreadContext& getContext() noexcept;

protected:
    static thread_local AnalyzerThreadContext analyzerContext;

    ScopeManager& globalScopeManager;
    mutable std::mutex exceptionMtx;
    std::unordered_map<std::string, std::vector<std::string>>& semanticErrors;
    const std::string& globalError;

    StatementAnalyzer statementAnalyzer;

    void checkParameter(const std::vector<std::unique_ptr<ASTParameter>>& _parameters, const std::string& functionName);
    void defineParameters(const std::vector<std::unique_ptr<ASTParameter>>& _parameters);
    void checkBody(const std::vector<std::unique_ptr<ASTStatement>>& _body);

    bool alwaysReturns(const ASTNode* _construct) const noexcept;
};

#endif