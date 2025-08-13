#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <mutex>

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"

/*
    Semantic analysis of abstract syntax tree
*/
class Analyzer{
    public:
        
        Analyzer(ScopeManager& scopeManager);

        void semanticCheck(const ASTProgram* _program);

    protected:
        // detection of undefined/redefined variables/functions, type checking
        ScopeManager globalScopeManager;
        // function analyzed by the thread, bool whether it returns, local scope managers for functions
        static thread_local AnalyzerThreadContext analyzerContext;

        mutable std::mutex exceptionMtx;
        mutable std::unordered_map<std::string, std::vector<std::string>> semanticErrors;
        
        static constexpr std::string globalError{ "__global" };
        
        void checkSemanticErrors(const ASTProgram* _functions) const;

        // function
        void checkFunctionSignatures(const ASTProgram* _program);
        void startFunctionCheck(const ASTProgram* _program);
        void checkFunction(const ASTFunction* _function);
        void checkParameter(const std::vector<std::unique_ptr<ASTParameter>>& _parameters, const std::string& functionName);
        void defineParameters(const std::vector<std::unique_ptr<ASTParameter>>& _parameters);
        void checkBody(const std::vector<std::unique_ptr<ASTStatement>>& _body);

        // statements
        void checkStatement(const ASTStatement* _statement);
        void checkVariable(const ASTVariable* _variable);
        void checkPrintfStatement(const ASTPrintfSt* _printf);
        void checkIfStatement(const ASTIfSt* _if);
        void checkWhileStatement(const ASTWhileSt* _while);
        void checkForStatement(const ASTForSt* _for);
        void checkDoWhileStatement(const ASTDoWhileSt* _dowhile);
        void checkSwitchStatement(const ASTSwitchSt* _switch);
        
        template<typename T>
        void checkSwitchStatementCases(const ASTSwitchSt* _switch);
        void checkCompoundStatement(const ASTCompoundSt* _compound);
        void checkAssignmentStatement(const ASTAssignSt* _assignment);
        void checkReturnStatement(const ASTReturnSt* _return);

        // expressions
        void checkNumericalExpression(ASTExpression* _numexp);
        Types getNumericalExpressionType(ASTExpression* _numexp);
        void checkRelationalExpression(ASTExpression* _relexp);
        bool checkID(ASTId* _id);
        void checkLiteral(const ASTLiteral* _literal) const;
        void checkFunctionCall(ASTFunctionCall* _functionCall);
        void checkArgument(const ASTFunctionCall* _functionCall);

        bool alwaysReturns(const ASTNode* _construct) const noexcept;

};

#endif
