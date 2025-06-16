#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <mutex>

#include "../common/abstract-syntax-tree/ASTree.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"

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

        void checkSemanticErrors(const ASTree* _functionList) const;

        // function
        void checkFunctionSignatures(const ASTree* _function);
        void startFunctionCheck(const ASTree* _functionList);
        void checkFunction(const ASTree* _function);
        void checkParameter(ASTree* _parameters, const std::string& functionName);
        void defineParameters(const ASTree* _parameters);
        void checkBody(const ASTree* _body);

        // constructs
        void checkConstruct(const ASTree* _construct);

        // variable
        void checkVariable(const ASTree* _variable);

        // statements
        void checkStatement(const ASTree* _statement);
        void checkPrintfStatement(const ASTree* _printf);
        void checkIfStatement(const ASTree* _if);
        void checkWhileStatement(const ASTree* _while);
        void checkForStatement(const ASTree* _for);
        void checkDoWhileStatement(const ASTree* _dowhile);
        void checkSwitchStatement(const ASTree* _switch);
        
        template<typename T>
        void checkSwitchStatementCases(const ASTree* _switch);
        void checkCompoundStatement(const ASTree* _compound);
        void checkAssignmentStatement(const ASTree* _assignment);
        void checkReturnStatement(const ASTree* _return);

        // expressions
        void checkNumericalExpression(ASTree* _numexp);
        Types getNumericalExpressionType(ASTree* _numexp);
        void checkRelationalExpression(const ASTree* _relexp);
        bool checkID(ASTree* _id);
        void checkLiteral(const ASTree* _literal) const;
        void checkFunctionCall(ASTree* _functionCall);
        void checkArgument(const ASTree* _functionCall);

};

#endif
