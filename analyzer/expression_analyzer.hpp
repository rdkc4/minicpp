#ifndef EXPRESSION_ANALYZER_HPP
#define EXPRESSION_ANALYZER_HPP

#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"

class ExpressionAnalyzer {
public:
    ExpressionAnalyzer(ScopeManager& scopeManager);

    void checkNumericalExpression(ASTExpression* _numexp);
    Types getNumericalExpressionType(ASTExpression* _numexp);
    bool checkID(ASTId* _id);
    void checkLiteral(const ASTLiteral* _literal) const;

private:
    ScopeManager& globalScopeManager;

    void checkRelationalExpression(ASTExpression* _relexp);
    void checkFunctionCall(ASTFunctionCall* _functionCall);
    void checkArgument(const ASTFunctionCall* _functionCall);
};

#endif