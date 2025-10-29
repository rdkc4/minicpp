#ifndef STATEMENT_ANALYZER_HPP
#define STATEMENT_ANALYZER_HPP

#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "expression_analyzer.hpp"

class StatementAnalyzer {
public:
    StatementAnalyzer(ScopeManager& scopeManager);

    void checkStatement(const ASTStatement* _statement);

protected:
    ScopeManager& globalScopeManager;
    ExpressionAnalyzer expressionAnalyzer;

    // statements
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
};

#endif