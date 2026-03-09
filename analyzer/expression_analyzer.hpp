#ifndef EXPRESSION_ANALYZER_HPP
#define EXPRESSION_ANALYZER_HPP

#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../common/abstract-syntax-tree/ast_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"

/**
 * @class ExpressionAnalyzer
 * @brief semantic analysis of the AST expression
*/
class ExpressionAnalyzer {
public:
    /** 
     * @brief Creates the instance of the analyzer specialized for expressions
     * @param scopeManager - reference to the global scope manager
    */
    ExpressionAnalyzer(ScopeManager& scopeManager);

    /** 
     * @brief determines the type of the expression node
     * @param _numexp - pointer to an ast expression
    */
    void checkNumericalExpression(ASTExpr* _numexp);

    /** 
     * @brief semantic check of the numerical expression node
     * @param _numexp - pointer to an ast expression
     * @returns type of the expression node
    */
    Type checkNumericalExpressionType(ASTExpr* _numexp);

    /** 
     * @brief semantic check of the id node
     * @param _id - pointer to an id node
     * @returns true if id exists, false otherwise
    */
    bool checkID(ASTIdExpr* _id);

    /** 
     * @brief semantic check of the literal node
     * @param _literal - pointer to a literal node
    */
    void checkLiteral(const ASTLiteralExpr* _literal) const;

    /** 
     * @brief semantic check of the relational expression
     * @param _relexp - pointer to a relational expression
    */
    void checkRelationalExpression(ASTExpr* _relexp);

    /** 
     * @brief semantic check of the function call
     * @param _functionCall - pointer to an ast function-call node
    */
    void checkFunctionCall(ASTFunctionCallExpr* _functionCall);

    /** 
     * @brief semantic check of the arguments of the function call
     * @param _functionCall - const pointer to an ast function-call node
    */
    void checkArgument(const ASTFunctionCallExpr* _functionCall);

private:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;
};

#endif