#ifndef ANALYZER_TEST_HPP
#define ANALYZER_TEST_HPP

#include <string>
#include <vector>
#include <cassert>

#include "../../analyzer/analyzer.hpp"

class AnalyzerTest : public Analyzer {
    public:
        AnalyzerTest(ScopeManager& scopeManager) : Analyzer{ scopeManager }{}

        ScopeManager& getScopeManager() noexcept {
            return globalScopeManager;
        }

        const std::string& getGlobalErrLabel() const noexcept {
            return globalError;
        }

        const std::vector<std::string>& getErrors(const std::string& func) const noexcept{
            assert(semanticErrors.find(func) != semanticErrors.end());
            return semanticErrors.at(func);
        }
};

class FunctionAnalyzerTest : public FunctionAnalyzer {
public:
    FunctionAnalyzerTest(ScopeManager& scopeManager, std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err) 
        : FunctionAnalyzer(scopeManager, semErrors, err) {}

    void testCheckFunctionSignatures(const ASTProgram* _program){
        checkFunctionSignatures(_program);
    }

    void testCheckFunction(const ASTFunction* _function){
        checkFunction(_function);
    }

    std::vector<std::string>& getErrors(const std::string& funcName){
        assert(semanticErrors.contains(funcName));
        return semanticErrors[funcName];
    }

    const std::string& getGlobalErrLabel() const noexcept {
        return globalError;
    }

    void initErrorsEmpty(const std::string& func){
        semanticErrors[func] = {};
    }
};

class StatementAnalyzerTest : public StatementAnalyzer {
public:
    StatementAnalyzerTest(ScopeManager& scopeManager) : StatementAnalyzer(scopeManager) {}

    void testCheckVariable(const ASTVariable* _variable){
        checkVariable(_variable);
    } 

    void testCheckCompoundStatement(const ASTCompoundSt* _compound){
        checkCompoundStatement(_compound);
    }

    void testCheckForStatement(const ASTForSt* _for){
        checkForStatement(_for);
    }

    void testCheckSwitchStatement(const ASTSwitchSt* _switch){
        checkSwitchStatement(_switch);
    }

    AnalyzerThreadContext& getContext() noexcept {
        return FunctionAnalyzer::getContext();
    }
};

class ExpressionAnalyzerTest : public ExpressionAnalyzer {

};

#endif
