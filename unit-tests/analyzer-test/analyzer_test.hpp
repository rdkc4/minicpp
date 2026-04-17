#ifndef ANALYZER_TEST_HPP
#define ANALYZER_TEST_HPP

#include <string>
#include <vector>
#include <cassert>

#include "../../analyzer/analyzer.hpp"

class AnalyzerTest : public semantics::Analyzer {
    public:
        AnalyzerTest(semantics::ScopeManager& scopeManager, ThreadPool& threadPool) 
            : semantics::Analyzer{ scopeManager, threadPool }{}

        semantics::ScopeManager& getScopeManager() noexcept {
            return globalScopeManager;
        }

        const char* getGlobalErrLabel() const noexcept {
            return globalError;
        }

        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(semanticErrors.find(func) != semanticErrors.end());
            return semanticErrors.at(func);
        }

        semantics::AnalyzerThreadContext& getContext() noexcept {
            return analyzerContext;
        }
};

#endif