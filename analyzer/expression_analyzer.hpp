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
     * @param numericalExpr - pointer to an ast expression
    */
    void checkNumericalExpr(ASTExpr* numericalExpr);

    /** 
     * @brief semantic check of the numerical expression node
     * @param numericalExpr - pointer to an ast expression
     * @returns type of the expression node
    */
    Type checkNumericalExprType(ASTExpr* numericalExpr);

    /** 
     * @brief semantic check of the id node
     * @param idExpr - pointer to an id node
     * @returns true if id exists, false otherwise
    */
    bool checkIDExpr(ASTIdExpr* idExpr);

    /** 
     * @brief semantic check of the literal node
     * @param literalExpr - pointer to a literal node
    */
    void checkLiteralExpr(const ASTLiteralExpr* literalExpr) const;

    /** 
     * @brief semantic check of the relational expression
     * @param relationalExpr - pointer to a relational expression
    */
    void checkRelationalExpr(ASTExpr* relationalExpr);

    /** 
     * @brief semantic check of the function call
     * @param callExpr - pointer to an ast function-call node
    */
    void checkFunctionCallExpr(ASTFunctionCallExpr* callExpr);

    /** 
     * @brief semantic check of the arguments of the function call
     * @param callExpr - const pointer to an ast function-call node
    */
    void checkArgument(const ASTFunctionCallExpr* callExpr);

private:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;
};

#endif