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
     * @brief semantic check for any ast statement
     * @param _statement - const pointer to a statement node
    */
    void checkStatement(const ASTStmt* _statement);

    /** 
     * @brief getter for the context of the function that owns statements
     * @return reference to a context of the analyzer thread
    */
    AnalyzerThreadContext& getContext() noexcept;

    /** 
     * @brief semantic check for variable declaration
     * @param _variable - const pointer to a variable node
    */
    void checkVariable(const ASTVariableDeclStmt* _variable);

    /** 
     * @brief semantic check for if-statement
     * @param _if - const pointer to an if-statement node
    */
    void checkIfStatement(const ASTIfStmt* _if);

    /** 
     * @brief semantic check for while-statement
     * @param _while - const pointer to a while-statement node
    */
    void checkWhileStatement(const ASTWhileStmt* _while);

    /** 
     * @brief semantic check for for-statement
     * @param _for - const pointer to a for-statement node
    */
    void checkForStatement(const ASTForStmt* _for);

    /** 
     * @brief semantic check for do-while-statement
     * @param _dowhile - const pointer to a do-while-statement node
    */
    void checkDoWhileStatement(const ASTDoWhileStmt* _dowhile);

    /** 
     * @brief semantic check for compound-statement
     * @param _compound - const pointer to a compound-statement node
    */
    void checkCompoundStatement(const ASTCompoundStmt* _compound);

    /** 
     * @brief semantic check for assignment-statement
     * @param _assignment - const pointer to an assignment-statement node
    */
    void checkAssignmentStatement(const ASTAssignStmt* _assignment);

    /** 
     * @brief semantic check for return-statement
     * @param _return - const pointer to a return-statement node
    */
    void checkReturnStatement(const ASTReturnStmt* _return);

    /** 
     * @brief semantic check for function-call-statement
     * @param _call - const pointer to a function-call-statement node
    */
    void checkFunctionCallStatement(const ASTFunctionCallStmt* _call);

    /** 
     * @brief semantic check for switch-statement
     * @param _switch - const pointer to a switch-statement node
    */
    void checkSwitchStatement(const ASTSwitchStmt* _switch);

    /** 
     * @brief semantic check for the cases of the switch-statement
     * @param _switch - const pointer to a switch-statement node
    */
    template<typename T>
    void checkSwitchStatementCases(const ASTSwitchStmt* _switch){
        auto& analyzerContext = getContext();
        std::unordered_set<T> set;
        Type expectedType{ std::is_same<T, int>::value ? Type::INT : Type::UNSIGNED };

        for(const auto& _case : _switch->getCases()){
            expressionAnalyzer.checkLiteral(_case->getLiteral());

            T val;
            // case type check
            Type type{ _case->getLiteral()->getType() };
            if(type != expectedType){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                        _case->getToken().line, _case->getToken().column, typeToString.at(expectedType), typeToString.at(type))
                );
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(_case->getLiteral()->getToken().value) 
                                                                : std::stoul(_case->getLiteral()->getToken().value))) != set.end()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                        _case->getToken().line, _case->getToken().column, _case->getLiteral()->getToken().value)
                );
            }
            else{
                for(const auto& _statement : _case->getSwitchBlock()->getStatements()){
                    checkStatement(_statement.get());
                }
                set.insert(val);
            }
        }
        if(_switch->hasDefault()){
            for(const auto& _statement : _switch->getDefault()->getSwitchBlock()->getStatements()){
                checkStatement(_statement.get());
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