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

        void testCheckFunctionSignatures(const ASTree* _functionList){
            checkFunctionSignatures(_functionList);
        }

        void testCheckFunction(const ASTree* _function){
            checkFunction(_function);
        }

        void testCheckVariable(const ASTree* _variable){
            checkVariable(_variable);
        }

        void testCheckForStatement(const ASTree* _for){
            checkForStatement(_for);
        }

        void testCheckSwitchStatement(const ASTree* _switch){
            checkSwitchStatement(_switch);
        }

        void testCheckCompoundStatement(const ASTree* _compound){
            checkCompoundStatement(_compound);
        }

};

#endif
