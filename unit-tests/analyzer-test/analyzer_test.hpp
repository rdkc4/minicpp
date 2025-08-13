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

        void initErrorsEmpty(const std::string& func){
            semanticErrors[func] = {};
        }

        AnalyzerThreadContext& getAnalyzerContext() const noexcept{
            return analyzerContext;
        }

        void testCheckFunctionSignatures(const ASTProgram* _program){
            checkFunctionSignatures(_program);
        }

        void testCheckFunction(const ASTFunction* _function){
            checkFunction(_function);
        }

        void testCheckVariable(const ASTVariable* _variable){
            checkVariable(_variable);
        }

        void testCheckForStatement(const ASTForSt* _for){
            checkForStatement(_for);
        }

        void testCheckSwitchStatement(const ASTSwitchSt* _switch){
            checkSwitchStatement(_switch);
        }

        void testCheckCompoundStatement(const ASTCompoundSt* _compound){
            checkCompoundStatement(_compound);
        }

};

#endif
