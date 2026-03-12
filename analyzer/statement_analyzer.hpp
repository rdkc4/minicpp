#ifndef STATEMENT_ANALYZER_HPP
#define STATEMENT_ANALYZER_HPP

#include <unordered_set>
#include <string>
#include <format>

#include "../common/abstract-syntax-tree/ast_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_while_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_for_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_return_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_stmt.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"
#include "expression_analyzer.hpp"

/** 
 * @class StatementAnalyzer
 * @brief semantic analysis of the AST statement
*/
class StatementAnalyzer {
public:
    /** 
     * @brief Creates the instance of the analyzer specialized for statements
     * @param scopeManager - reference to the global scope manager
    */
    StatementAnalyzer(ScopeManager& scopeManager);

    /** 
     * @brief getter for the context of the function that owns statements
     * @return reference to a context of the analyzer thread
    */
    AnalyzerThreadContext& getContext() noexcept;

    /** 
     * @brief semantic check for any ast statement
     * @param stmt - const pointer to a statement node
    */
    void checkStmt(const ASTStmt* stmt);

    /** 
     * @brief semantic check for variable declaration
     * @param variableDecl - const pointer to a variable declaration node
    */
    void checkVariableDeclStmt(const ASTVariableDeclStmt* variableDecl);

    /** 
     * @brief semantic check for if-statement
     * @param ifStmt - const pointer to an if-statement node
    */
    void checkIfStmt(const ASTIfStmt* ifStmt);

    /** 
     * @brief semantic check for while-statement
     * @param whileStmt - const pointer to a while-statement node
    */
    void checkWhileStmt(const ASTWhileStmt* whileStmt);

    /** 
     * @brief semantic check for for-statement
     * @param forStmt - const pointer to a for-statement node
    */
    void checkForStmt(const ASTForStmt* forStmt);

    /** 
     * @brief semantic check for do-while-statement
     * @param dowhileStmt - const pointer to a do-while-statement node
    */
    void checkDoWhileStmt(const ASTDoWhileStmt* dowhileStmt);

    /** 
     * @brief semantic check for compound-statement
     * @param compoundStmt - const pointer to a compound-statement node
    */
    void checkCompoundStmt(const ASTCompoundStmt* compoundStmt);

    /** 
     * @brief semantic check for assignment-statement
     * @param assignStmt - const pointer to an assignment-statement node
    */
    void checkAssignStmt(const ASTAssignStmt* assignStmt);

    /** 
     * @brief semantic check for return-statement
     * @param returnStmt - const pointer to a return-statement node
    */
    void checkReturnStmt(const ASTReturnStmt* returnStmt);

    /** 
     * @brief semantic check for function-call-statement
     * @param callStmt - const pointer to a function-call-statement node
    */
    void checkFunctionCallStmt(const ASTFunctionCallStmt* callStmt);

    /** 
     * @brief semantic check for switch-statement
     * @param switchStmt - const pointer to a switch-statement node
    */
    void checkSwitchStmt(const ASTSwitchStmt* switchStmt);

    /** 
     * @brief semantic check for the cases of the switch-statement
     * @param switchStmt - const pointer to a switch-statement node
    */
    template<typename T>
    void checkCaseStmts(const ASTSwitchStmt* switchStmt){
        auto& analyzerContext = getContext();
        std::unordered_set<T> set;
        Type expectedType{ std::is_same<T, int>::value ? Type::INT : Type::UNSIGNED };

        for(const auto& caseStmt : switchStmt->getCaseStmts()){
            expressionAnalyzer.checkLiteralExpr(caseStmt->getLiteralExpr());

            T val;
            // case type check
            Type type{ caseStmt->getLiteralExpr()->getType() };
            if(type != expectedType){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                        caseStmt->getToken().line, caseStmt->getToken().column, typeToString.at(expectedType), typeToString.at(type))
                );
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(caseStmt->getLiteralExpr()->getToken().value) 
                                                                : std::stoul(caseStmt->getLiteralExpr()->getToken().value))) != set.end()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                        caseStmt->getToken().line, caseStmt->getToken().column, caseStmt->getLiteralExpr()->getToken().value)
                );
            }
            else{
                for(const auto& stmt : caseStmt->getSwitchBlockStmt()->getStmts()){
                    checkStmt(stmt.get());
                }
                set.insert(val);
            }
        }
        if(switchStmt->hasDefaultStmt()){
            for(const auto& stmt : switchStmt->getDefaultStmt()->getSwitchBlockStmt()->getStmts()){
                checkStmt(stmt.get());
            }
        }
    }

private:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;

    /// analyzer specialized for the expression analysis
    ExpressionAnalyzer expressionAnalyzer;

};

#endif