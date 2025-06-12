#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include "../common/abstract-syntax-tree/ASTree.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"
#include <unordered_map>

/*
    Semantic analysis of abstract syntax tree
*/
class Analyzer{
    public:
        
        Analyzer(ScopeManager& scopeManager);

        void semanticCheck(const ASTree* root);

    private:
        // detection of undefined/redefined variables/functions, type checking
        ScopeManager globalScopeManager;
        // function analyzed by the thread, bool whether it returns, local scope managers for functions
        static thread_local AnalyzerThreadContext analyzerContext;

        mutable std::mutex exceptionMtx;
        mutable std::unordered_map<std::string, std::vector<std::string>> semanticErrors;

        void checkSemanticErrors(const ASTree* node) const;

        // function
        void checkFunctionSignatures(const ASTree* node);
        void startFunctionCheck(const ASTree* flist);
        void checkFunction(const ASTree* node);
        void checkParameter(ASTree* node, const std::string& functionName);
        void defineParameters(ASTree* node) const;
        void checkBody(const ASTree* node);

        // constructs
        void checkConstruct(const ASTree* node);

        // variable
        void checkVariable(const ASTree* node);

        // statements
        void checkStatement(const ASTree* node);
        void checkIfStatement(const ASTree* node);
        void checkWhileStatement(const ASTree* node);
        void checkForStatement(const ASTree* node);
        void checkDoWhileStatement(const ASTree* node);
        void checkSwitchStatement(const ASTree* node);
        
        template<typename T>
        void checkSwitchStatementCases(const ASTree* node);
        void checkCompoundStatement(const ASTree* node);
        void checkAssignmentStatement(const ASTree* node) const;
        void checkReturnStatement(const ASTree* node);

        // expressions
        void checkNumericalExpression(ASTree* node) const;
        Types getNumericalExpressionType(ASTree* node) const;
        void checkRelationalExpression(const ASTree* node) const;
        bool checkID(ASTree* node) const;
        void checkLiteral(const ASTree* node) const;
        void checkFunctionCall(ASTree* node) const;
        void checkArgument(const ASTree* node) const;

};

#endif
