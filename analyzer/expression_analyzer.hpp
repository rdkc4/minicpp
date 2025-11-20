#ifndef EXPRESSION_ANALYZER_HPP
#define EXPRESSION_ANALYZER_HPP

#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"

/**
 * @class ExpressionAnalyzer
 * @brief semantic analysis of the AST expression
*/
class ExpressionAnalyzer {
public:
    ExpressionAnalyzer(ScopeManager& scopeManager);

    /** 
     * @brief determines the type of the expression node
     * @param _numexp - pointer to an ast expression
     * @returns void
    */
    void checkNumericalExpression(ASTExpression* _numexp);

    /** 
     * @brief semantic check of the numerical expression node
     * @param _numexp - pointer to an ast expression
     * @returns type of the expression node
    */
    Types checkNumericalExpressionType(ASTExpression* _numexp);

    /** 
     * @brief semantic check of the id node
     * @param _id - pointer to an id node
     * @returns true if id exists, false otherwise
    */
    bool checkID(ASTId* _id);

    /** 
     * @brief semantic check of the literal node
     * @param _literal - pointer to a literal node
     * @returns void
    */
    void checkLiteral(const ASTLiteral* _literal) const;

private:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;

    /** 
     * @brief semantic check of the relational expression
     * @param _relexp - pointer to a relational expression
     * @returns void
    */
    void checkRelationalExpression(ASTExpression* _relexp);

    /** 
     * @brief semantic check of the function call
     * @param _functionCall - pointer to an ast function-call node
     * @returns void
    */
    void checkFunctionCall(ASTFunctionCall* _functionCall);

    /** 
     * @brief semantic check of the arguments of the function call
     * @param _functionCall - const pointer to an ast function-call node
     * @returns void
    */
    void checkArgument(const ASTFunctionCall* _functionCall);
};

#endif